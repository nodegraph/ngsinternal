#include <native/scripts.h>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>

#include <iostream>
#include <cassert>

Scripts::Scripts() {
}

QString Scripts::get_script(const std::string& filename) {
  if (!QFileInfo::exists(filename.c_str())) {
    assert(false);
  }

  QFile file(filename.c_str());
  if (!file.open(QFile::ReadOnly | QFile::Text)) {
    return "";
  }
  QTextStream stream(&file);
  return stream.readAll();
}

QString Scripts::get_event_recorder_script() {
  return get_script(":/browser/eventrecorder.js");
}

QString Scripts::get_event_retriever_script() {
  return get_script(":/browser/getevent.js");
}

QString Scripts::get_error_retriever_script() {
  return get_script(":/browser/geterrors.js");
}

QString Scripts::get_start_recording_script() {
  return get_script(":/browser/startrecording.js");
}

QString Scripts::get_stop_recording_script() {
  return get_script(":/browser/stoprecording.js");
}

QString Scripts::get_test_script() {
  return get_script(":/javascript/test2.js");
}

QString Scripts::get_channel_script() {
  return get_script(":/qtwebchannel/qwebchannel.js");
}

void Scripts::test() {
  emit test_signal();
}
