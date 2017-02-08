#include <base/objectmodel/entity.h>
#include <base/utils/path.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

#include <components/computes/inputcompute.h>
#include <components/computes/jsonutils.h>

#include <guicomponents/comms/guitypes.h>
#include <guicomponents/computes/postnodecompute.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/quick/nodegraphmanipulator.h>

//#include <QtCore/QVariant>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExpression>
#include <QtQml/QQmlContext>

namespace ngs {

PostNodeCompute::PostNodeCompute(Entity* entity):
  QObject(),
  Compute(entity, kDID()) {
  // In derived classes the input and output param names should be set here.
}

PostNodeCompute::~PostNodeCompute() {
}

void PostNodeCompute::create_inputs_outputs(const EntityConfig& config) {
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
    c.unconnected_value = 0;
    create_input("post_type", c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "";
    create_input("title", c);
  }
  {
      EntityConfig c = config;
      c.expose_plug = false;
      c.unconnected_value = "";
      create_input("property_path", c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = true;
    create_output("out", c);
  }
}

const QJsonObject PostNodeCompute::_hints = PostNodeCompute::init_hints();
QJsonObject PostNodeCompute::init_hints() {
  QJsonObject m;
  add_hint(m, "in", GUITypes::HintKey::DescriptionHint, "The main input from which a property will be posted.");

  add_hint(m, "post_type", GUITypes::HintKey::DescriptionHint, "The type of post to make.");
  add_hint(m, "post_type", GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::PostType));

  add_hint(m, "title", GUITypes::HintKey::DescriptionHint, "The title for the post.");
  add_hint(m, "property_path", GUITypes::HintKey::DescriptionHint, "The path to the property in the input.");
  return m;
}

bool PostNodeCompute::update_state() {
  internal();
  Compute::update_state();

  // Copy the input to the output.
  QJsonValue in = _inputs->get_input_value("in");
  set_output("out", in);

  // Get the title text.
  QString title = _inputs->get_input_value("title").toString();

  // Get the post type.
  int post_type =  _inputs->get_input_value("post_type").toInt();

  // Get the path to the property in the in input.
  QString path_string = _inputs->get_input_value("property_path").toString();
  Path path(Path::split_string(path_string.toStdString()));

  // Get the value of the property.
  QJsonObject in_obj = _inputs->get("in")->get_output("out").toObject();
  QJsonValue value = JSONUtils::extract_value(in_obj, path);

  // Wrap the value in an object.
  QJsonObject obj;
  obj.insert("value", value);

  // Post the value.
  _manipulator->send_post_value_signal(post_type, title, obj);
  return true;
}

}
