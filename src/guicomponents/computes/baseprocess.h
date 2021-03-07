#pragma once
#include <guicomponents/computes/guicomputes_export.h>

#include <QtCore/QProcess>
#include <QtCore/QObject>

namespace ngs {

class GUICOMPUTES_EXPORT BaseProcess : public QObject {
Q_OBJECT
 public:
  explicit BaseProcess();
  virtual ~BaseProcess();

  virtual void start();
  virtual void stop();
  virtual bool is_running() const;
  virtual long long get_pid() const;

 signals:
  void errored(const QString& error);

 protected:

  // Setup.
  void set_process_name(const QString& process_name);
  void set_program(const QString& program);
  void set_args(const QStringList& args);
  void set_working_dir(const QString& dir);

  // Comms.
  void disconnect_stderr();
  void disconnect_stdout();

 protected slots:
  virtual void on_started();
  virtual void on_error(QProcess::ProcessError error);
  virtual void on_state_changed(QProcess::ProcessState);
  virtual void on_stderr();
  virtual void on_stdout();

 private:
  QProcess* _process;

 protected:
  QString _process_name;
  QString _program;
  QStringList _args;
  QString _working_dir;

  bool _show_stream_activity;
  QString _last_stderr;
  QString _last_stdout;

};

}

