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

DownloadVideoProcess::DownloadVideoProcess()
    : BaseProcess(),
      _max_width(-1),
      _max_height(-1),
      _max_filesize(-1),
      _progress_regex(_progress_pattern),
      _filename_regex(_filename_pattern){
  set_process_name("download video");
}

DownloadVideoProcess::~DownloadVideoProcess() {
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

void DownloadVideoProcess::on_error(QProcess::ProcessError error) {
  BaseProcess::on_error(error);
  switch (error) {
    case QProcess::ProcessError::FailedToStart: {
      emit errored(get_pid(), "failed to start");
      break;
    }
    case QProcess::ProcessError::Crashed: {
      emit errored(get_pid(), "crashed");
      break;
    }
    case QProcess::ProcessError::Timedout: {
      emit errored(get_pid(), "timed out");
      break;
    }
    case QProcess::ProcessError::WriteError: {
      emit errored(get_pid(), "write error");
      break;
    }
    case QProcess::ProcessError::ReadError: {
      emit errored(get_pid(), "read error");
      break;
    }
    case QProcess::ProcessError::UnknownError: {
      emit errored(get_pid(), "unknown error");
      break;
    }
  }
}

void DownloadVideoProcess::on_state_changed(QProcess::ProcessState state) {
  if (state == QProcess::NotRunning) {
    emit finished(get_pid());
  }
}

void DownloadVideoProcess::on_read_standard_error() {
  BaseProcess::on_read_standard_error();
}

const char* DownloadVideoProcess::_filename_pattern = "\\[download\\]\\s*Destination:\\s*(.+)";
const char* DownloadVideoProcess::_progress_pattern = "\\[download\\]\\s*(.*)";

void DownloadVideoProcess::on_read_standard_output() {
  BaseProcess::on_read_standard_output();

  // Example filename message from std out.
  // [download] Destination: the best video ever.mp4
  // Note we use lastIndexIn because a number of these [download] message may arrive concatenated together.
  int pos = _filename_regex.lastIndexIn(_last_stdout);
  if (pos >= 0) {
    QStringList list = _filename_regex.capturedTexts();
    // list[0] contains the full match. The other elements contain the submatches.
    QString filename = list[1];
    // Stip off any parent directories on the filename.
    QStringList splits = filename.split(QDir::separator(), QString::SkipEmptyParts);
    QString tail = splits[splits.size()-1];
    tail = tail.simplified(); // strip whitespace from the front and back
    // Emit signal.
    emit started(get_pid(), tail);
    return;
  }

  // Example progress message from std out.
  // [download] 100% of 35.77MiB in 00:05
  // [download] 0.0% of 87.34MiB at 857.15KiB/s ETA 01:44
  // Note we use lastIndexIn because a number of these [download] message may arrive concatenated together.
  pos = _progress_regex.lastIndexIn(_last_stdout);
  if (pos >= 0) {
    QStringList list = _progress_regex.capturedTexts();
    // list[0] contains the full match. The other elements contain the submatches.
    QString msg = list[1];
    msg = msg.simplified(); // strip whitespace from the front and back
    // Emit signal.
    emit progress(get_pid(), msg);
    return;
  }


}

void DownloadVideoProcess::start() {
  if (is_running()) {
    return;
  }

  // Determine platform specific program.
#if (ARCH == ARCH_WINDOWS)
  QString program = "youtube-dl.exe";
#elif (ARCH == ARCH_MACOS)
  QString program = "youtube-dl";
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
