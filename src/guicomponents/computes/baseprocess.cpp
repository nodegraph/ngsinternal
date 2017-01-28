#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <base/objectmodel/appconfig.h>
#include <guicomponents/computes/baseprocess.h>
#include <guicomponents/comms/message.h>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication.h>
#include <QtCore/QFile>

namespace ngs {

BaseProcess::BaseProcess()
    : QObject(NULL),
      _process(NULL) {
}

BaseProcess::~BaseProcess() {
  stop();
}

void BaseProcess::set_process_name(const QString& process_name) {
  _process_name = process_name;
}

void BaseProcess::set_program(const QString& program) {
  _program = program;
}

void BaseProcess::set_args(const QStringList& args) {
  _args = args;
}

void BaseProcess::on_started() {
  //qDebug() << "process has started";
}

void BaseProcess::on_error(QProcess::ProcessError error) {
  qDebug() << "process error: " << error << "\n";
}

void BaseProcess::on_state_changed(QProcess::ProcessState state) {
  if (state == QProcess::NotRunning) {
  }
}

void BaseProcess::on_read_standard_error() {
  QDebug debug = qDebug();
  debug.noquote();

  // Dump any std errors from the process.
  _last_stderr = _process->readAllStandardError();
  if (!_last_stderr.isEmpty()) {
    debug << "stderr from: " << _process_name << "\n";
    debug << "------------------------------------------------\n";
    debug << _last_stderr;
    debug << "------------------------------------------------\n";
  }
}

void BaseProcess::on_read_standard_output() {
  QDebug debug = qDebug();
  debug.noquote();

  // Dump any std output from the process.
  _last_stdout = _process->readAllStandardOutput();
  if (!_last_stdout.isEmpty()) {
    debug << "stdout from " << _process_name << "\n";
    debug << "------------------------------------------------\n";
    debug << _last_stdout;
    debug << "------------------------------------------------\n";
  }
}

void BaseProcess::start() {
  if (is_running()) {
    return;
  }

  _process = new_ff QProcess(this);

  // Connect to signals.
  connect(_process, SIGNAL(started()), this, SLOT(on_started()));
  connect(_process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(on_error(QProcess::ProcessError)));
  connect(_process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(on_state_changed(QProcess::ProcessState)));
  connect(_process, SIGNAL(readyReadStandardError()), this, SLOT(on_read_standard_error()));
  connect(_process, SIGNAL(readyReadStandardOutput()), this, SLOT(on_read_standard_output()));

  _process->setProgram(_program);
  _process->setArguments(_args);
  QString folder = AppConfig::get_app_bin_dir();
  _process->setWorkingDirectory(folder);
  _process->start();

  // We wait processing events until it's running.
  while(!is_running()) {
    qApp->processEvents();
  }
}

void BaseProcess::stop() {
  if (_process) {
    _process->kill();
    delete_ff(_process);
  }
}

bool BaseProcess::is_running() const {
  if (_process) {
    if (_process->state() == QProcess::Running) {
      return true;
    }
  }
  return false;
}

long long BaseProcess::get_pid() const {
  if (_process) {
    return _process->processId();
  }
  return 0;
}

}
