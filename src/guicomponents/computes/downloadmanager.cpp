#include <guicomponents/computes/downloadmanager.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/appconfig.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/computes/downloadvideoprocess.h>
#include <guicomponents/comms/licensechecker.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtCore/QDir>

namespace ngs {


const int DownloadManager::kCheckInterval = 1000;

DownloadManager::DownloadManager(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _manipulator(this),
      _license_checker(this),
      _file_model(this),
      _num_rows(0) {
  get_dep_loader()->register_fixed_dep(_manipulator, Path());
  get_dep_loader()->register_fixed_dep(_license_checker, Path());
  get_dep_loader()->register_fixed_dep(_file_model, Path());

  // Setup the poll timer.
  _poll_timer.setSingleShot(false);
  _poll_timer.setInterval(kCheckInterval);
  connect(&_poll_timer,SIGNAL(timeout()),this,SLOT(on_poll()));
  _poll_timer.start();
}

DownloadManager::~DownloadManager() {
  _poll_timer.stop();
  for (auto &p: _processes) {
    delete_ff(p.second);
  }
  _processes.clear();
}

void DownloadManager::on_poll() {
  // If the number of running processes is less than the max, then start another.
  int max = _file_model->get_max_concurrent_downloads();
  if (get_num_running() < max) {
    for (auto &i: _processes) {
      if (!i.second->is_running()) {
        i.second->start();
        break;
      }
    }
  }
}

int DownloadManager::get_num_running() const {
  int count = 0;
  for (auto &i: _processes) {
    if (i.second->is_running()) {
      count += 1;
    }
  }
  return count;
}

void DownloadManager::cancel_download(long long id) {
  destroy_process(id);
  remove_id_from_map(id);
}

void DownloadManager::download(const QString& url, const QString& download_dir, int max_width, int max_height, int max_filesize) {
  DownloadVideoProcess *p = new_ff DownloadVideoProcess();

  // Connect to signals.
  connect(p, SIGNAL(started(const QString&)), this, SLOT(on_started(const QString&)));
  connect(p, SIGNAL(progress(const  QString&, const  QString&, const  QString&, const  QString&)), this, SLOT(on_progress(const  QString&, const  QString&, const  QString&, const  QString&)));
  connect(p, SIGNAL(complete(const  QString&, const  QString&, const  QString&)), this, SLOT(on_complete(const  QString&, const  QString&, const  QString&)));
  connect(p, SIGNAL(merged(const QString&)), this, SLOT(on_merged(const QString&)));
  connect(p, SIGNAL(finished()), this, SLOT(on_finished()));
  connect(p, SIGNAL(errored(const QString&)), this, SLOT(on_errored(const QString&)));

  // Setup the arguments.
  QString path = download_dir;

  // Combine the directory parameter with the default donwload directory.
  QDir dir(path);
  if (dir.isRelative()) {
    path = _file_model->get_default_download_dir() + QDir::separator() + dir.path();
  } else {
    path = dir.path();
  }

  // Setup the download process.
  p->set_dir(path);
  p->set_url(url);
  p->set_max_width(max_width);
  p->set_max_height(max_height);
  p->set_max_filesize(max_filesize);

  // Queue the process.
  _processes.insert({p->get_id(),p});

  // Emit queued signal.
  emit download_queued(p->get_id(), url, path);
  // Update our id_to_row.
  _id_to_row[p->get_id()] = _num_rows++;
}

int DownloadManager::get_sender_id() const {
  QObject* obj = sender();
  DownloadVideoProcess *p = static_cast<DownloadVideoProcess*>(obj);
  return p->get_id();
}

int DownloadManager::get_row(int id) const {
  return _id_to_row.at(id);
}

void DownloadManager::destroy_process(long long id) {
  DownloadVideoProcess *p = _processes[id];
  _processes.erase(id);
  // Let Qt destroy this when convenient.
  p->deleteLater();
  // The following delete causes a crash.
  //delete_ff(p);
}

void DownloadManager::remove_id_from_map(int id) {
  // Remove the id from the map.
  int row = _id_to_row[id];
  _id_to_row.erase(id);
  // Update the rows on the remaining elements.
  for (auto &iter: _id_to_row) {
    if (iter.second >= row) {
      iter.second -= 1;
    }
  }
  // Update the total number of rows we have.
  _num_rows -= 1;
}

// ---------------------------------------------------------------------------------------------
// Relaying signal from video download process.
// ---------------------------------------------------------------------------------------------

void DownloadManager::on_started(const QString& filename) {
  internal();
  int id = get_sender_id();
  int row = get_row(id);
  emit download_started(row, filename);
}

void DownloadManager::on_progress(const  QString& percentage, const QString& file_size, const QString& speed, const QString& eta) {
  internal();
  int id = get_sender_id();
  int row = get_row(id);
  emit download_progress(row, percentage, file_size, speed, eta);
}

void DownloadManager::on_complete(const  QString& percentage, const QString& file_size, const QString& time) {
  internal();
  int id = get_sender_id();
  int row = get_row(id);
  emit download_complete(row, percentage, file_size, time);
}

void DownloadManager::on_merged(const QString& merged_filename) {
  internal();
  int id = get_sender_id();
  int row = get_row(id);
  emit download_merged(row, merged_filename);
}

void DownloadManager::on_errored(const QString& error) {
  internal();
  int id = get_sender_id();
  int row = get_row(id);
  destroy_process(id);
  remove_id_from_map(id);
  emit download_errored(row, error);
}

void DownloadManager::on_finished() {
  internal();
  int id = get_sender_id();
  int row = get_row(id);
  destroy_process(id);
  remove_id_from_map(id);
  emit download_finished(row);
}

// ---------------------------------------------------------------------------------------------
// File Reveal Functionality.
// ---------------------------------------------------------------------------------------------



void DownloadManager::reveal_approximate_filename(const QString& dir, const QString &filename) {
  QString best_filename = find_best_matching_file(dir, filename);
  DownloadManager::reveal_file_on_platform(dir, best_filename);
}

void DownloadManager::reveal_exact_filename(const QString& dir, const QString &filename) {
  DownloadManager::reveal_file_on_platform(dir, filename);
}

QString DownloadManager::find_best_matching_file(const QString& dir, const QString& filename) {
  // Find the base filename, which is the part up to the first period.
  QFileInfo info(filename);
  QString base_name = info.baseName();

  // Create our name filters.
  QStringList name_filters;
  name_filters.append(base_name+".*");

  // Grab the files matching the name filters.
  QDir d(dir);
  QFileInfoList files = d.entryInfoList(name_filters, QDir::Files);

  // Find the file with the largest byte size.
  qint64 largest = 0;
  QString largest_file = "";
  for (QFileInfo &info : files) {
    info.size();
    if (info.size() > largest) {
      largest = info.size();
      largest_file = info.fileName();
    }
  }
  return largest_file;
}

void DownloadManager::reveal_file_on_platform(const QString& dir, const QString &filename) {
  if (dir.isEmpty()) {
    return;
  }

#if (ARCH == ARCH_WINDOWS)
  QString cmd = "explorer.exe";
  QStringList args;
  if (!filename.isEmpty()) {
    args.append("/select,");
  }
  args.append(QDir::toNativeSeparators(dir) + QDir::separator() + filename);
  QProcess::startDetached(cmd, args);
#elif (ARCH == ARCH_MACOS)
  {
    QStringList args;
    args.append("-e");
    QString cmd = "tell application \"Finder\" to reveal POSIX file \"" + dir + "/" + filename + "\"";
    args.append(cmd);
    QProcess::execute("/usr/bin/osascript", args);
  }
  {
    QStringList args;
    args.append("-e");
    QString cmd = "tell application \"Finder\" to activate";
    args.append(cmd);
    QProcess::execute("/usr/bin/osascript", args);
  }
#endif

}

}
