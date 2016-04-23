#include <base/objectmodel/entity.h>
#include <components/api/dataapi.h>

#include <base/utils/path.h>
#include <components/computes/compute.h>


namespace ngs {

DataAPI::DataAPI():
  _current_data_root(NULL){
}

DataAPI::~DataAPI() {
}

// The current data tree root.
void DataAPI::set_current_data_root(DataLocation* root) {
  _current_data_root = root;
}

// Data access from root.
void DataAPI::set_data_value(const QString& data_name, const QVariant& data_value, const QString& rel_path) {
  _current_data_root->set_data_value(data_name,data_value,Path(rel_path.toStdWString()));
}

QVariant DataAPI::get_data_value(const QString& data_name, const QString& rel_path) {
  return _current_data_root->get_data_value(data_name,Path(rel_path.toStdWString()));
}

QStringList DataAPI::get_data_names(const QString& rel_path) {
  return _current_data_root->get_data_names(Path(rel_path.toStdWString()));
}

bool DataAPI::has_data_name(const QString& data_name, const QString& rel_path) {
  return _current_data_root->has_data_name(data_name,Path(rel_path.toStdWString()));
}

void DataAPI::delete_data_value(const QString& data_name, const QString& rel_path) {
  _current_data_root->delete_data_value(data_name,Path(rel_path.toStdWString()));
}

// Location access from root.

bool DataAPI::has_location(const QString& rel_path) {
  return _current_data_root->has_location(Path(rel_path.toStdWString()));
}

void DataAPI::delete_location(const QString& rel_path) {
  return _current_data_root->delete_location(Path(rel_path.toStdWString()));
}


}
