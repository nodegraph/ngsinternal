#pragma once
#include <guicomponents/computes/guicomputes_export.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/componentids.h>
#include <base/objectmodel/dep.h>



#include <QtCore/QProcess>
#include <QtCore/QObject>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>


namespace ngs {

class Message;
class DownloadVideoProcess;
class BaseNodeGraphManipulator;

// This class communicates with the nodejs process.
class GUICOMPUTES_EXPORT DownloadManager : public QObject, public Component {
Q_OBJECT
 public:
  COMPONENT_ID(DownloadManager, DownloadManager)

  explicit DownloadManager(Entity* parent);
  virtual ~DownloadManager();

  Q_INVOKABLE void download_on_the_side(const QString& url);
  void download(int msg_id, const QJsonObject& args);
  void stop(long long pid);

signals:
 void download_queued(long long pid, const QString& url);
 void download_started(long long pid, const QString& filename);
 void download_progress(long long pid, const  QString& progress);
 void download_finished(long long pid);
 void download_errored(long long pid, const QString& error);

 private slots:
  void on_queued_side_download(long long pid, const QString& url);
  void on_queued(long long pid, const QString& url);
  void on_started(long long pid, const QString& filename);
  void on_progress(long long pid, const  QString& progress);
  void on_errored(long long pid, const QString& error);
  void on_finished(long long pid);

 private:
  void destroy_process(long long pid);

  // Our fixed dependencies.
  Dep<BaseNodeGraphManipulator> _manipulator;

  std::unordered_map<long long, DownloadVideoProcess*> _processes;

  // Msg id for the last download request.
  int _last_msg_id;
};

}

