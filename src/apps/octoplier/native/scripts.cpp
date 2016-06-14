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
  return get_script(":/browser/content.js");
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

QString Scripts::get_jquery() {
  return get_script(":/browser/jquery-2.2.4.js");
}

QString Scripts::get_jquery_simulate() {
  return get_script(":/browser/jquery.simulate.js");
}

void Scripts::test() {
  emit test_signal();
}
