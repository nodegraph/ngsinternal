#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <base/objectmodel/appconfig.h>
#include <guicomponents/computes/javaprocess.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <guicomponents/comms/message.h>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication.h>
#include <QtCore/QFile>

namespace ngs {

JavaProcess::JavaProcess(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _manipulator(this),
      _process(NULL),
      _msg_id(-1){
  get_dep_loader()->register_fixed_dep(_manipulator, Path());
}

JavaProcess::~JavaProcess() {
  stop_process();
}

void JavaProcess::on_started() {
  internal();
  //qDebug() << "process has started";
}

void JavaProcess::on_error(QProcess::ProcessError error) {
  internal();
  qDebug() << "process error: " << error << "\n";
}

void JavaProcess::on_state_changed(QProcess::ProcessState state) {
  internal();
  if (state == QProcess::NotRunning) {
    Message response(true, true);
    response.insert(Message::kID, _msg_id);
    _manipulator->receive_message(response.to_string());
  }
}

void JavaProcess::on_read_standard_error() {
  internal();
  QDebug debug = qDebug();
  debug.noquote();

  // Dump any std errors from the process.
  QString error(_process->readAllStandardError());
  if (!error.isEmpty()) {
    debug << "------------------ java error ------------------\n";
    debug << error;
    debug << "------------------------------------------------\n";
  }
}

void JavaProcess::on_read_standard_output() {
  internal();
  QDebug debug = qDebug();
  debug.noquote();

  //debug << "nodejs state: " << _process->state();
  QString output(_process->readAllStandardOutput());

  // Dump any std output from the process.
  if (!output.isEmpty()) {
    debug << "------------------ java out --------------------\n";
    debug << output;
    debug << "------------------------------------------------\n";
  }

  _manipulator->receive_message(output);
}

// -----------------------------------------------------------------
// Private Methods.
// -----------------------------------------------------------------

void JavaProcess::set_msg_id(int msg_id) {
  _msg_id = msg_id;
}

void JavaProcess::send_msg(const Message& msg) {
  QByteArray bytes = msg.to_string().toUtf8();
  QString num_bytes = QString::number(bytes.count());
  _process->write(bytes);
  _process->write("\n");
}

void JavaProcess::start_process() {
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
  // Set the working directory.
  QString folder = AppConfig::get_app_bin_dir();
  _process->setWorkingDirectory(folder);

  // Set the powershell binary.
  QString java_binary_path = "java";
  _process->setProgram(java_binary_path);

  QStringList args;
  args.append("-cp");
  // Our main jar.
  QString jars = "jcomm.jar";
  // Gson.
  jars += ";gson/gson-2.8.0.jar";
  // Selenium.
  jars+=";selenium-java-3.0.1/client-combined-3.0.1-nodeps.jar";
  jars+=";selenium-java-3.0.1/lib/client-combined-3.0.1-nodeps.jar";
  jars+=";selenium-java-3.0.1/lib/cglib-nodep-3.2.4.jar";
  jars+=";selenium-java-3.0.1/lib/commons-codec-1.10.jar";
  jars+=";selenium-java-3.0.1/lib/commons-exec-1.3.jar";
  jars+=";selenium-java-3.0.1/lib/commons-io-2.5.jar";
  jars+=";selenium-java-3.0.1/lib/commons-lang3-3.4.jar";
  jars+=";selenium-java-3.0.1/lib/commons-logging-1.2.jar";
  jars+=";selenium-java-3.0.1/lib/cssparser-0.9.20.jar";
  jars+=";selenium-java-3.0.1/lib/gson-2.3.1.jar";
  jars+=";selenium-java-3.0.1/lib/guava-19.0.jar";
  jars+=";selenium-java-3.0.1/lib/hamcrest-core-1.3.jar";
  jars+=";selenium-java-3.0.1/lib/hamcrest-library-1.3.jar";
  jars+=";selenium-java-3.0.1/lib/htmlunit-2.23.jar";
  jars+=";selenium-java-3.0.1/lib/htmlunit-core-js-2.23.jar";
  jars+=";selenium-java-3.0.1/lib/httpclient-4.5.2.jar";
  jars+=";selenium-java-3.0.1/lib/httpcore-4.4.4.jar";
  jars+=";selenium-java-3.0.1/lib/httpmime-4.5.2.jar";
  jars+=";selenium-java-3.0.1/lib/javax.servlet-api-3.1.0.jar";
  jars+=";selenium-java-3.0.1/lib/jetty-io-9.2.13.v20150730.jar";
  jars+=";selenium-java-3.0.1/lib/jetty-util-9.2.13.v20150730.jar";
  jars+=";selenium-java-3.0.1/lib/jna-4.1.0.jar";
  jars+=";selenium-java-3.0.1/lib/jna-platform-4.1.0.jar";
  jars+=";selenium-java-3.0.1/lib/junit-4.12.jar";
  jars+=";selenium-java-3.0.1/lib/neko-htmlunit-2.23.jar";
  jars+=";selenium-java-3.0.1/lib/netty-3.5.7.Final.jar";
  jars+=";selenium-java-3.0.1/lib/phantomjsdriver-1.3.0.jar";
  jars+=";selenium-java-3.0.1/lib/sac-1.3.jar";
  jars+=";selenium-java-3.0.1/lib/serializer-2.7.2.jar";
  jars+=";selenium-java-3.0.1/lib/websocket-api-9.2.15.v20160210.jar";
  jars+=";selenium-java-3.0.1/lib/websocket-client-9.2.15.v20160210.jar";
  jars+=";selenium-java-3.0.1/lib/websocket-common-9.2.15.v20160210.jar";
  jars+=";selenium-java-3.0.1/lib/xalan-2.7.2.jar";
  jars+=";selenium-java-3.0.1/lib/xercesImpl-2.11.0.jar";
  jars+=";selenium-java-3.0.1/lib/xml-apis-1.4.01.jar";
  args.append(jars);
  // The main class.
  args.append("BrowserController");
  // The arguments to the program.
  args.append(AppConfig::get_user_app_dir());
  args.append("8093");

  _process->setArguments(args);
  _process->start();

#elif (ARCH == ARCH_MACOS)
  // Set the working directory.
  QString folder = AppConfig::get_app_bin_dir();
  _process->setWorkingDirectory(folder);

  // Set the powershell binary.
  QString java_binary_path = "java";
  _process->setProgram(java_binary_path);

  QStringList args;
  args.append("-cp");
  args.append("gson-2.8.0.jar:jcomm.jar");
  args.append("BrowserController");

  _process->setArguments(args);
  _process->start();
#endif


  // We wait processing events until it's running.
  while(!is_running()) {
    qApp->processEvents();
  }
}

void JavaProcess::stop_process() {
  external();
  if (_process) {
    _process->kill();
    delete_ff(_process);
  }
}

bool JavaProcess::is_running() const {
  external();
  if (_process) {
    if (_process->state() == QProcess::Running) {
      return true;
    }
  }
  return false;
}

}
