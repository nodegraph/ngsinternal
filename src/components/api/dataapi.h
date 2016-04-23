#pragma once
#include <components/api/system_api_export.h>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>


namespace ngs {

class Entity;
class DataLocation;


class SYSTEM_API_EXPORT DataAPI: public QObject {
  Q_OBJECT
 public:
  DataAPI();
  virtual ~DataAPI();

 public slots:

   // The current data tree root.
   void set_current_data_root(DataLocation* root);

   // Data access from root.
   void set_data_value(const QString& data_name, const QVariant& data_value, const QString& rel_path = QString()); // this will build the path if immediate nodes don't exist.
   QVariant get_data_value(const QString& data_name, const QString& rel_path = QString());
   QStringList get_data_names(const QString& rel_path = QString());
   bool has_data_name(const QString& data_name, const QString& rel_path = QString());
   void delete_data_value(const QString& data_name, const QString& rel_path = QString());

   // Location access from root.
   bool has_location(const QString& rel_path);
   void delete_location(const QString& rel_path);

 private:
   DataLocation* _current_data_root;
};

}
