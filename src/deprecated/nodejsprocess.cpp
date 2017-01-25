#include <guicomponents/comms/nodejsprocess.h>
#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <base/objectmodel/appconfig.h>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication.h>

namespace ngs {

NodeJSProcess::NodeJSProcess(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _process(NULL),
      _nodejs_port_regex("port:(\\d+)"){
}

NodeJSProcess::~NodeJSProcess() {
  stop_process();
}

void NodeJSProcess::on_started() {
  internal();
  //qDebug() << "process has started";
}

void NodeJSProcess::on_error(QProcess::ProcessError error) {
  internal();
  qDebug() << "process error: " << error << "\n";
}

void NodeJSProcess::on_state_changed(QProcess::ProcessState state) {
  internal();
}

void NodeJSProcess::on_read_standard_error() {
  internal();
  QDebug debug = qDebug();
  debug.noquote();

  // Dump any std errors from the process.
  QString error(_process->readAllStandardError());
  if (!error.isEmpty()) {
    debug << "------------------nodejs err--------------------\n";
    debug << error;
    debug << "------------------------------------------------\n";
  }
}

void NodeJSProcess::on_read_standard_output() {
  internal();
  QDebug debug = qDebug();
  debug.noquote();

  //debug << "nodejs state: " << _process->state();
  QString output(_process->readAllStandardOutput());

  // Scan the standard output for the nodejs port number, if we haven't found it yet.
  if (_nodejs_port.isEmpty()) {
    int pos = _nodejs_port_regex.indexIn(output);
    if (pos >= 0) {
      QStringList list = _nodejs_port_regex.capturedTexts();
      assert(list.size() == 2);
      _nodejs_port = list[1];
    }
  }

  // Dump any std output from the process.
  if (!output.isEmpty()) {
    debug << "------------------nodejs out--------------------\n";
    debug << output;
    debug << "------------------------------------------------\n";
  }
}

// -----------------------------------------------------------------
// Private Methods.
// -----------------------------------------------------------------

void NodeJSProcess::start_process() {
  external();
  if (is_running()) {
    return;
  }

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
  _process->setProgram(QString("node.exe"));
#elif (ARCH == ARCH_MACOS)
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  QString node_modules_path;// = AppConfig::get_app_bin_dir();
  node_modules_path = "../Resources/nodejs/lib/node_modules";

  qDebug() << "node modules path: " << node_modules_path << "\n";
  env.insert("NODE_PATH", node_modules_path); // Add an environment variable
  _process->setProcessEnvironment(env);

  //setenv("NODE_PATH", node_modules_path.toStdString().c_str(), 1);

  
  qDebug() << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  qDebug() << "env list is: " << env.toStringList();
  qDebug() << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  qDebug() << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  qDebug() << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  qDebug() << "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  qDebug() << "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb";
  qDebug() << "ccccccccccccccccccccccccccccccccccccccccccccccccc";
  qDebug() << "ddddddddddddddddddddddddddddddddddddddddddddddddd";
  qDebug() << "eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee";

  QString node_path;// = AppConfig::get_app_bin_dir();
  node_path = "../Resources/nodejs/bin/node";
  qDebug() << "node path: " << node_path << "\n";
  _process->setProgram(node_path);
#endif
  // Set the working directory.
  QString folder = AppConfig::get_app_bin_dir();
  _process->setWorkingDirectory(folder);

  // Set the arguments.
  QStringList list("chb.js");
  qDebug() << "user app dir: " << AppConfig::get_user_app_dir() << "\n";
  list.append(AppConfig::get_user_app_dir());
  _process->setArguments(list);
  _process->start();


  // We wait processing events until it's running.
  while(!is_running()) {
    qApp->processEvents();
  }

  // Wait for the nodejs port to get sent back to us through the standard output stream.
  while(get_nodejs_port().isEmpty()) {
    qApp->processEvents();
  }
}

void NodeJSProcess::stop_process() {
  external();
  if (_process) {
    _process->kill();
    delete_ff(_process);
  }
}

bool NodeJSProcess::is_running() const {
  external();
  if (_process) {
    if (_process->state() == QProcess::Running) {
      return true;
    }
  }
  return false;
}

}
