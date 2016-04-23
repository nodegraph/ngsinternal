#include <base/objectmodel/entity.h>
#include <components/api/paramapi.h>

#include <base/utils/path.h>
#include <components/computes/compute.h>


namespace ngs {

ParamAPI::ParamAPI(Entity* ng_root):
  _ng_root(ng_root),
  _current_data_root(NULL){
}

ParamAPI::~ParamAPI() {

}

void ParamAPI::set_current_node_path(const QString& node_path) {
  _current_node_path = node_path;
}

QVariant ParamAPI::get_param_value(const QString& param_name, const QString& node_path) {
//  // Get the root group.
//  Group* group = _ng_root->get_group();
//
//  // Get the node.
//
//  Path path;
//  if (node_path.isEmpty()) {
//    path.set(_current_node_path.toStdWString());
//  } else {
//    path.set(node_path.toStdWString());
//  }
//
//  Node* node = group->get_node(path);
//  if (!node) {
//    return QVariant();
//  }
//
//  // Get the computer.
//  Computer* computer = node->switch_to_data_computer();
//  if (!computer) {
//    return QVariant();
//  }
//
//  // Get the param.
//  Param* param = computer->get_param(param_name.toStdWString());
//  if (!param) {
//    return QVariant();
//  }
//
//  // Get the tuple vector.
//  const TupleVector& tuple_vec = param->get_evaluated();
//  return tuple_vector_to_variant(tuple_vec);
}

}
