#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <base/objectmodel/appconfig.h>
#include <guicomponents/computes/acceptsaveprocess.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <guicomponents/comms/message.h>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication.h>
#include <QtCore/QFile>

namespace ngs {

AcceptSaveProcess::AcceptSaveProcess(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _manipulator(this),
      _process(NULL),
      _msg_id(-1){
  get_dep_loader()->register_fixed_dep(_manipulator, Path());
}

AcceptSaveProcess::~AcceptSaveProcess() {
  stop_process();
}

void AcceptSaveProcess::on_started() {
  internal();
  //qDebug() << "process has started";
}

void AcceptSaveProcess::on_error(QProcess::ProcessError error) {
  internal();
  qDebug() << "process error: " << error << "\n";
}

void AcceptSaveProcess::on_state_changed(QProcess::ProcessState state) {
  internal();
  if (state == QProcess::NotRunning) {
    Message response(ReceiverType::Platform, true, true);
    response.insert(Message::kID, _msg_id);
    _manipulator->receive_message(response.to_string());
  }
}

void AcceptSaveProcess::on_read_standard_error() {
  internal();
  QDebug debug = qDebug();
  debug.noquote();

  // Dump any std errors from the process.
  QString error(_process->readAllStandardError());
  if (!error.isEmpty()) {
    debug << "------------------accept save process error --------------------\n";
    debug << error;
    debug << "------------------------------------------------\n";
  }
}

void AcceptSaveProcess::on_read_standard_output() {
  internal();
  QDebug debug = qDebug();
  debug.noquote();

  QString output(_process->readAllStandardOutput());

  // Dump any std output from the process.
  if (!output.isEmpty()) {
    debug << "------------------accept save process--------------------\n";
    debug << output;
    debug << "------------------------------------------------\n";
  }
}

// -----------------------------------------------------------------
// Private Methods.
// -----------------------------------------------------------------

void AcceptSaveProcess::start_process(int msg_id) {
  external();
  if (is_running()) {
    return;
  }

  _msg_id = msg_id;

  // Create a process.
  delete_ff(_process);
  _process = new_ff QProcess(this);

  // Connect to signals.
  connect(_process, SIGNAL(started()), this, SLOT(on_started()));
  connect(_process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(on_error(QProcess::ProcessError)));
  connect(_process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(on_state_changed(QProcess::ProcessState)));
  connect(_process, SIGNAL(readyReadStandardError()), this, SLOT(on_read_standard_error()));
  connect(_process, SIGNAL(readyReadStandardOutput()), this, SLOT(on_read_standard_output()));

  // Set the executable.
#if (ARCH == ARCH_WINDOWS)
  // Set the powershell binary.
  QString power_shell_path = "C:/Windows/system32/WindowsPowerShell/v1.0/powershell.exe";
  _process->setProgram(power_shell_path);

  // Get the power shell script.
  QFile script(":/scripts/acceptsave.ps1");
  if (!script.open(QIODevice::ReadOnly | QIODevice::Text))
  {
      qDebug() << "Unable to open file: " << script.fileName() << " error: " << script.errorString() << "\n";
  }

  QTextStream stream(&script);
  QString script_contents = stream.readAll();

  QStringList args;
  args.append("-Command");
  args.append(script_contents);
  _process->setArguments(args);
  _process->start();


#elif (ARCH == ARCH_MACOS)
  // Set the osascript binary.
  QString osascript_path = "/usr/bin/osascript";
  _process->setProgram(osascript_path);

  // Get the osascript script.
  QFile script(":/scripts/acceptsave.scpt");
  if (!script.open(QIODevice::ReadOnly | QIODevice::Text))
  {
      qDebug() << "Unable to open file: " << script.fileName() << " error: " << script.errorString() << "\n";
  }

  QTextStream stream(&script);
  QString script_contents = stream.readAll();

  QStringList args;
  args << "-l" << "AppleScript";

  _process->start();
  _process->write(script_contents.toUtf8());
  _process->closeWriteChannel();
#endif

  // Set the working directory.
//  QString folder = AppConfig::get_app_bin_dir();
//  _process->setWorkingDirectory(folder);


  // We wait processing events until it's running.

  while(!is_running()) {
    qApp->processEvents();
  }
}

void AcceptSaveProcess::stop_process() {
  external();
  if (_process) {
    _process->kill();
    delete_ff(_process);
  }
}

bool AcceptSaveProcess::is_running() const {
  external();
  if (_process) {
    if (_process->state() == QProcess::Running) {
      return true;
    }
  }
  return false;
}

}
