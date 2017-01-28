#pragma once
#include <guicomponents/computes/guicomputes_export.h>

#include <QtCore/QProcess>
#include <QtCore/QObject>

namespace ngs {

// This class communicates with the nodejs process.
class GUICOMPUTES_EXPORT BaseProcess : public QObject {
Q_OBJECT
 public:
  explicit BaseProcess();
  virtual ~BaseProcess();

  virtual void start();
  virtual void stop();
  virtual bool is_running() const;
  virtual long long get_pid() const;

 protected:

  // Setup.
  void set_process_name(const QString& process_name);
  void set_program(const QString& program);
  void set_args(const QStringList& args);

 protected slots:
  virtual void on_started();
  virtual void on_error(QProcess::ProcessError error);
  virtual void on_state_changed(QProcess::ProcessState);
  virtual void on_read_standard_error();
  virtual void on_read_standard_output();

 private:
  QProcess* _process;

 protected:
  QString _process_name;
  QString _program;
  QStringList _args;

  bool _show_stream_activity;
  QString _last_stderr;
  QString _last_stdout;

};

}

