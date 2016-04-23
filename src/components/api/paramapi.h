#pragma once
#include <components/api/system_api_export.h>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>


namespace ngs {

class Entity;
class DataLocation;


class SYSTEM_API_EXPORT ParamAPI: public QObject {
  Q_OBJECT
 public:
  ParamAPI(Entity* ng_root);
  virtual ~ParamAPI();

 public slots:

   // The current node context.
   void set_current_node_path(const QString& node_path);

   // The getters.
   QVariant get_param_value(const QString& param_name, const QString& node_path = QString());

 private:
   Entity* _ng_root;
   QString _current_node_path;
   DataLocation* _current_data_root;
};

}
