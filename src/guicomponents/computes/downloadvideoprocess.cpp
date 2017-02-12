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
      _max_width(-1),
      _max_height(-1),
      _max_filesize(-1),
      _already_downloaded_regex(_already_downloaded_pattern),
      _progress_regex(_progress_pattern),
      _filename_regex(_filename_pattern){
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
const char* DownloadVideoProcess::_progress_pattern = "(.*)";

void DownloadVideoProcess::on_stdout() {
  if (_started) {
    //disconnect_stdout();
    //return;
  }

  BaseProcess::on_stdout();
  QStringList lines = _last_stdout.split('\n', QString::SkipEmptyParts);

  for (QString & line: lines) {

    // Multiple message lines from our process may get jammed together so we need to split them.
    QStringList splits = line.split("[download]", QString::SkipEmptyParts);
    for (QString &split: splits) {

      if (split.startsWith("Deleting original file")) {
        continue;
      }

      if (split.startsWith("[ffmpeg]")) {
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
        QString base_name = info.baseName();
        // Emit signal.
        _started = true;
        emit started(base_name);
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
        QString base_name = info.baseName();
        // Emit signal.
        _started = true;
        emit started(base_name);
        continue;
      }

      // Example progress message from std out.
      // [download] 100% of 35.77MiB in 00:05
      // [download] 0.0% of 87.34MiB at 857.15KiB/s ETA 01:44
      // Note we use lastIndexIn because a number of these [download] message may arrive concatenated together.
      pos = _progress_regex.indexIn(split);
      if (pos >= 0) {
        QStringList list = _progress_regex.capturedTexts();
        // list[0] contains the full match. The other elements contain the submatches.
        QString msg = list[1];
        msg = msg.simplified(); // strip whitespace from the front and back
        // Emit signal.
        emit progress(msg);
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
  QString program = "./ngs_helper";
#endif

  // Set the program.
  set_program(program);

  QStringList args;
  args.append("-w"); // Don't overwrite existing files.
  args.append("-c"); // Continue downloads of partially downloaded files.
  args.append("--write-all-thumbnails"); // Write thumbnails to disk.

  // Set the preferred format.
  if ((_max_width > 0) || (_max_height > 0) || (_max_filesize > 0)) {
    args.append("-f");
    QString selector = "bestvideo";
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
    args.append(selector);
  }

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
