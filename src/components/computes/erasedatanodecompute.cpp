#include <base/objectmodel/entity.h>
#include <components/computes/erasedatanodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

EraseDataNodeCompute::EraseDataNodeCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

EraseDataNodeCompute::~EraseDataNodeCompute() {
}

void EraseDataNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_main_input(config);
  create_main_output(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "value";
    create_input("path", c);
  }
}

const QJsonObject EraseDataNodeCompute::_hints = EraseDataNodeCompute::init_hints();
QJsonObject EraseDataNodeCompute::init_hints() {
  QJsonObject m;
  add_main_input_hint(m);
  add_hint(m, "path", GUITypes::HintKey::DescriptionHint, "The path at which to erase data.");
  return m;
}

bool EraseDataNodeCompute::update_state() {
  Compute::update_state();

  // Our input object.
  QJsonObject in_obj = _inputs->get_main_input_object();

  // The source path.
  QString path_string = _inputs->get_input_string("path");
  Path path(Path::split_string(path_string.toStdString()));

  // Get the source value.
  QJsonValue src_value = JSONUtils::extract_value(in_obj, path, QJsonValue());

  // Embed the value.
  in_obj = JSONUtils::erase_value(in_obj, path);

  // Set the output.
  set_main_output(in_obj);
  return true;
}

}

