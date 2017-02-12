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

// This class communicates with the nodejs process.
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
  Q_INVOKABLE void download_on_the_side(const QString& url);
  void download(int msg_id, const QJsonObject& args);

  Q_INVOKABLE void reveal_file(const QString& dir, const QString &filename);
  static QString find_best_matching_file(const QString& dir, const QString& filename);
  static void reveal_file_on_platform(const QString& dir, const QString &filename);

signals:
 void download_queued(long long id, const QString& url, const QString& dest_dir);
 void download_started(long long id, const QString& filename);
 void download_progress(long long id, const  QString& progress);
 void download_finished(long long id);
 void download_errored(long long id, const QString& error);

 private slots:
  void on_poll();
  void on_started(const QString& filename);
  void on_progress(const  QString& progress);
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

  // Msg id for the last download request.
  int _last_msg_id;

  // Timer to check when is room to start running the next queued process.
  QTimer _poll_timer;

  // Maps the download id to the qml row.
  std::unordered_map<int, int> _id_to_row;
  int _num_rows;
  std::vector<int> _finished_ids;
};

}

