#include <guicomponents/comms/commutils.h>
#include <base/utils/path.h>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>

#include <QtCore/QJsonValue>
#include <QtCore/QJsonObject>

namespace ngs {

QStringList path_to_string_list(const Path& path) {
  const std::deque<std::string>& elems = path.get_elements();
  std::deque<std::string>::const_iterator iter;
  QStringList list;
  for (iter = elems.begin(); iter != elems.end(); ++iter) {
    list.push_back(iter->c_str());
  }
  return list;
}

Path string_list_to_path(const QStringList& list) {
  Path path;
  for (int i=0; i<list.size(); ++i) {
    path.push_back(list[i].toStdString());
  }
  return path;
}

Path var_list_to_path(const QVariantList& list) {
  Path path;
  for (int i=0; i<list.size(); ++i) {
    path.push_back(list[i].toString().toStdString());
  }
  return path;
}

}
