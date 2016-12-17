#include <base/objectmodel/entity.h>
#include <components/computes/erasedatanodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>
#include <guicomponents/comms/commtypes.h>

namespace ngs {

EraseDataNodeCompute::EraseDataNodeCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

EraseDataNodeCompute::~EraseDataNodeCompute() {
}

void EraseDataNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = true;
    c.unconnected_value = QJsonObject();
    create_input("in", c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "/result";
    create_input("path", c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = true;
    c.unconnected_value = QJsonObject();
    create_output("out", c);
  }
}

const QJsonObject EraseDataNodeCompute::_hints = EraseDataNodeCompute::init_hints();
QJsonObject EraseDataNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "in", HintKey::kDescriptionHint, "The main input object this node acts on.");
  add_hint(m, "path", HintKey::kDescriptionHint, "The path at which to erase data.");

  return m;
}

bool EraseDataNodeCompute::update_state() {
  Compute::update_state();

  // Our input object.
  QJsonObject in_obj = _inputs->get_input_value("in").toObject();

  // The source path.
  QString path_string = _inputs->get("path")->get_output("out").toString();
  Path path(Path::split_string(path_string.toStdString()));

  // Get the source value.
  QJsonValue src_value = JSONUtils::extract_value(in_obj, path, QJsonValue());

  // Embed the value.
  in_obj = JSONUtils::erase_value(in_obj, path);

  // Set the output.
  set_output("out", in_obj);
  return true;
}

}

