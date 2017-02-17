#include <base/objectmodel/entity.h>
#include <components/computes/copydatanodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

CopyDataNodeCompute::CopyDataNodeCompute(Entity* entity)
    : Compute(entity, kDID()) {
}

CopyDataNodeCompute::~CopyDataNodeCompute() {
}

void CopyDataNodeCompute::create_inputs_outputs(const EntityConfig& config) {
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
    c.unconnected_value = "value";
    create_input("source_path", c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "/copy";
    create_input("destination_path", c);
  }

  {
    EntityConfig c = config;
    c.expose_plug = true;
    c.unconnected_value = QJsonObject();
    create_output("out", c);
  }

}

const QJsonObject CopyDataNodeCompute::_hints = CopyDataNodeCompute::init_hints();
QJsonObject CopyDataNodeCompute::init_hints() {
  QJsonObject m;

  add_hint(m, "in", GUITypes::HintKey::DescriptionHint, "The input object within which to copy and paste a value.");
  add_hint(m, "source_path", GUITypes::HintKey::DescriptionHint, "The source path from which we want to copy data.");
  add_hint(m, "destination_path", GUITypes::HintKey::DescriptionHint, "The target path to which we want to paste data.");

  return m;
}

bool CopyDataNodeCompute::update_state() {
  Compute::update_state();

  // Our input object.
  QJsonObject in_obj = _inputs->get_input_object("in");

  // The source path.
  QString src_path_string = _inputs->get_input_string("source_path");
  Path src_path(Path::split_string(src_path_string.toStdString()));

  // The dest path.
  QString dest_path_string = _inputs->get_input_string("destination_path");
  Path dest_path(Path::split_string(dest_path_string.toStdString()));

  // Get the source value.
  QJsonValue src_value = JSONUtils::extract_value(in_obj, src_path, QJsonValue());

  // Embed the value.
  in_obj = JSONUtils::embed_value(in_obj, dest_path, src_value);

  // Set the output.
  set_output("out", in_obj);
  return true;
}

}

