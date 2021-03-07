#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/dep.h>



#include <QtCore/QProcess>
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>


namespace ngs {

class Message;
class DownloadVideoProcess;
class BaseNodeGraphManipulator;
class LicenseChecker;
class FileModel;

class GUICOMPUTES_EXPORT DownloadManager : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(DownloadManager, DownloadManager)

  static const int kCheckInterval;

  explicit DownloadManager(Entity* parent);
  virtual ~DownloadManager();

  // Info.
  int get_num_running() const;

  // Actions.
  Q_INVOKABLE void download(const QString& url,
                            const QString& download_dir = "",
                            bool merge_best_streams = false,
                            const QString& format = "mp4",
                            bool thumbnails = true,
                            int max_width = 0,
                            int max_height = 0,
                            int max_filesize = 0);

  Q_INVOKABLE void cancel_download(long long id);

  Q_INVOKABLE void reveal_approximate_filename(const QString& dir, const QString &filename);
  Q_INVOKABLE void reveal_exact_filename(const QString& dir, const QString &filename);

  static QString find_best_matching_file(const QString& dir, const QString& filename);
  static void reveal_file_on_platform(const QString& dir, const QString &filename);

signals:
 void download_queued(long long id, const QString& url, const QString& dest_dir);
 void download_started(long long id, const QString& filename);
 void download_progress(long long id, const  QString& percentage, const QString& file_size, const QString& speed, const QString& eta);
 void download_complete(long long id, const  QString& percentage, const QString& file_size, const QString& time);
 void download_merged(long long id, const QString& merged_filename);
 void download_finished(long long id);
 void download_errored(long long id, const QString& error);

 private slots:
  void on_poll();
  void on_started(const QString& filename);
  void on_progress(const  QString& percentage, const QString& file_size, const QString& speed, const QString& eta);
  void on_complete(const  QString& percentage, const QString& file_size, const QString& time);
  void on_merged(const QString& merged_filename);
  void on_errored(const QString& error);
  void on_finished();

 private:
  int get_sender_id() const;
  int get_row(int id) const;
  void destroy_process(long long id);
  void remove_id_from_map(int id);

  // Our fixed dependencies.
  Dep<BaseNodeGraphManipulator> _manipulator;
  Dep<LicenseChecker> _license_checker;
  Dep<FileModel> _file_model;

  // A map of ids to download video processes.
  std::unordered_map<long long, DownloadVideoProcess*> _processes;

  // Timer to check when is room to start running the next queued process.
  QTimer _poll_timer;

  // Maps the download id to the qml row.
  std::unordered_map<int, int> _id_to_row;
  int _num_rows;
  std::vector<int> _finished_ids;
};

}

