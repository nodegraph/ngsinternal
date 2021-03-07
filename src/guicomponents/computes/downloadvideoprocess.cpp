#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <base/objectmodel/appconfig.h>
#include <guicomponents/computes/downloadvideoprocess.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <guicomponents/comms/message.h>


#include <QtCore/QDebug>
#include <QtCore/QCoreApplication.h>
#include <QtCore/QFile>
#include <QtCore/QDir>

namespace ngs {

int DownloadVideoProcess::next_id = 0;

DownloadVideoProcess::DownloadVideoProcess()
    : BaseProcess(),
      _id(next_id++),
      _started(false),
      _merge(false),
      _format(""),
      _thumbnails(true),
      _max_width(-1),
      _max_height(-1),
      _max_filesize(-1),
      _already_downloaded_regex(_already_downloaded_pattern),
      _progress_regex(_progress_pattern),
      _complete_regex(_complete_pattern),
      _filename_regex(_filename_pattern),
      _merged_filename_regex(_merged_filename_pattern) {
  // Set our process name.
  set_process_name("download video");
  // Stop outputting stream activity.
  _show_stream_activity = false;
}

DownloadVideoProcess::~DownloadVideoProcess() {
}

int DownloadVideoProcess::get_id() const {
  return _id;
}

void DownloadVideoProcess::set_url(const QString& url) {
  _url = url;
}

void DownloadVideoProcess::set_dir(const QString& dir) {
  _dir = dir;
}

void DownloadVideoProcess::set_merge_best_streams(bool merge) {
  _merge = merge;
}

void DownloadVideoProcess::set_format(const QString& format) {
  _format = format;
}

void DownloadVideoProcess::set_thumbnails(bool thumbnails) {
  _thumbnails = thumbnails;
}

void DownloadVideoProcess::set_max_width(int w) {
  _max_width = w;
}

void DownloadVideoProcess::set_max_height(int h) {
  _max_height = h;
}

void DownloadVideoProcess::set_max_filesize(int s) {
  _max_filesize = s;
}

void DownloadVideoProcess::on_state_changed(QProcess::ProcessState state) {
  if (state == QProcess::NotRunning) {
    emit finished();
  }
}

void DownloadVideoProcess::on_stderr() {
  if (_started) {
    //disconnect_stderr();
    //return;
  }

  BaseProcess::on_stderr();
}

const char* DownloadVideoProcess::_already_downloaded_pattern ="(.+)\\s*has already been downloaded";
const char* DownloadVideoProcess::_filename_pattern = "Destination:\\s*(.+)";
const char* DownloadVideoProcess::_progress_pattern = "(.+)% of ~?(.+) at (.+) ETA (.+)"; //3.1% of ~25.47KiB at 530.22KiB/s ETA 00:05
const char* DownloadVideoProcess::_complete_pattern = "(.+)% of (.+) in (.+)"; //100% of 35.77MiB in 00:05
const char* DownloadVideoProcess::_merged_filename_pattern = "\\[ffmpeg\\] Merging formats into \"(.+)\"";

void DownloadVideoProcess::on_stdout() {
  if (_started) {
    //disconnect_stdout();
    //return;
  }

  BaseProcess::on_stdout();

  // Firstly, we split the lines on newlines.
  QStringList lines = _last_stdout.split('\n', QString::SkipEmptyParts);

  for (QString & line : lines) {

    //std::cerr << "received: " << line.toStdString() << "\n";

    // The merged filename comes back through the following output.
    // [ffmpeg] Merging formats into "BOMBTRACK AT RAD RACE'S  'LAST MAN STANDING 2017', BERLIN-209745487.mp4"
    if (line.startsWith("[ffmpeg] Merging formats into ")) {
      int pos = _merged_filename_regex.indexIn(line);
      if (pos >= 0) {
        QStringList list = _merged_filename_regex.capturedTexts();
        // list[0] contains the full match. The other elements contain the submatches.
        QString abs_path = list[1].trimmed();
        QFileInfo info(abs_path);
        QString dir = info.dir().path();
        QString merged_filename = info.fileName();
        emit merged (merged_filename);
        continue;
      }
    }

    // If the line starts with [download] there may be many jammed together as
    // the the process backspaces and writes over the current line
    // to show progress. We need to split them.
    QStringList splits = line.split("[download]", QString::SkipEmptyParts);
    for (QString &split: splits) {

      if (split.startsWith("Deleting original file")) {
        continue;
      }

      if (split.startsWith("WARNING: Requested formats are incompatible for merge and will be merged into mkv.")) {
        continue;
      }

      // [download] this is an awesome video.mp4 has already been downloaded
      int pos = _already_downloaded_regex.indexIn(split);
      if (pos >= 0) {
        QStringList list = _already_downloaded_regex.capturedTexts();
        // list[0] contains the full match. The other elements contain the submatches.
        QString abs_path = list[1].trimmed();
        QFileInfo info(abs_path);
        QString dir = info.dir().path();
        QString file_name = info.fileName();
        // Emit signal.
        _started = true;
        emit started(file_name);
        continue;
      }

      // Example filename message from std out.
      // [download] Destination: the best video ever.mp4
      // Note we use lastIndexIn because a number of these [download] message may arrive concatenated together.
      pos = _filename_regex.indexIn(split);
      if (pos >= 0) {
        QStringList list = _filename_regex.capturedTexts();
        // list[0] contains the full match. The other elements contain the submatches.
        QString abs_path = list[1].trimmed();
        QFileInfo info(abs_path);
        QString dir = info.dir().path();
        QString fileName = info.fileName();
        // Emit signal.
        _started = true;
        emit started(fileName);
        continue;
      }

      // Example progress messages.
      // [download] 3.1% of ~25.47KiB at 530.22KiB/s ETA 00:05
      // [download] 0.0% of 87.34MiB at 857.15KiB/s ETA 01:44
      pos = _progress_regex.indexIn(split);
      if (pos >= 0) {
        QStringList list = _progress_regex.capturedTexts();
        QString percentage = list[1].simplified();
        QString file_size = list[2].simplified();
        QString speed = list[3].simplified();
        QString eta = list[4].simplified();
        // list[0] contains the full match. The other elements contain the submatches.
        QString msg = list[1];
        msg = msg.simplified(); // strip whitespace from the front and back
        // Emit signal.
        emit progress(percentage, file_size, speed, eta);
        continue;
      }

      // Example final download complete message from std out.
      // [download] 100% of 35.77MiB in 00:05
      pos = _complete_regex.indexIn(split);
      if (pos >= 0) {
        QStringList list = _complete_regex.capturedTexts();
        QString percentage = list[1].simplified();
        QString file_size = list[2].simplified();
        QString time = list[3].simplified();
        // Emit signal.
        emit complete(percentage, file_size, time);
        continue;
      }
    }
  }
}

void DownloadVideoProcess::start() {
  if (is_running()) {
    return;
  }

  // Determine platform specific program.
#if (ARCH == ARCH_WINDOWS)
  QString program = "ngs_helper.dll";
#elif (ARCH == ARCH_MACOS)
  // Working dir on macos.
  QString working_dir = AppConfig::get_app_bin_dir();
  working_dir += "/../Resources/bin";
  set_working_dir(working_dir);
  // Program on macos.
  QString program = "./ngs_helper";
#else
  QString program = "ngs_unavailable_on_this_platform";
#endif

  // Make sure download directory exists.
  QFileInfo info(_dir);
  if (!info.exists()) {
    QDir().mkpath(_dir);
  }

  // Set the program.
  set_program(program);

  QStringList args;
  args.append("-w"); // Don't overwrite existing files.
  args.append("-c"); // Continue downloads of partially downloaded files.

  if (_thumbnails) {
    args.append("--write-all-thumbnails"); // Write thumbnails to disk.
  }

  args.append("-f");

  if (_merge) {
    QString selector = "bestvideo";

    // Set selection criteria.
    if (_max_width > 0) {
      selector += "[width<=" + QString::number(_max_width)+ "]";
    }
    if (_max_height > 0) {
      selector += "[height<=" + QString::number(_max_height) + "]";
    }
    if (_max_filesize > 0) {
      selector += "[filesize<=" + QString::number(_max_filesize) + "]";
    }
    selector += "+bestaudio/best";
    //selector = "\"" + selector + "\"";
    args.append(selector);

    std::cerr << "Selector111 is: " << selector.toStdString() << "\n";

    // Set the filename format.
    {
      args.append("-o");
      QString filename = _dir;
      if (!filename.isEmpty()) {
        filename += QDir::separator();
      }
      filename += "%(title)s-%(id)s.%(ext)s";
      args.append(filename);
    }
  } else {
    QString selector = "best";

    // Set selection criteria.
    if (!_format.isEmpty()) {
      selector += "[ext=" + _format + "]";
    }
    if (_max_width > 0) {
      selector += "[width<=" + QString::number(_max_width)+ "]";
    }
    if (_max_height > 0) {
      selector += "[height<=" + QString::number(_max_height) + "]";
    }
    if (_max_filesize > 0) {
      selector += "[filesize<=" + QString::number(_max_filesize) + "]";
    }
    selector += "/best";
    //selector = "\"" + selector + "\"";
    args.append(selector);

    std::cerr << "Selector222 is: " << selector.toStdString() << "\n";

    // Set the filename format.
    {
      args.append("-o");
      QString filename = _dir;
      if (!filename.isEmpty()) {
        filename += QDir::separator();
      }
      filename += "%(title)s-%(id)s.%(ext)s";
      args.append(filename);
    }
  }

  args.append(_url);

  // Set the args.
  set_args(args);

  // Start.
  BaseProcess::start();
}

const QString& DownloadVideoProcess::get_filename() const {
  return _filename;
}

}
