#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <base/utils/path.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <components/computes/inputcompute.h>
#include <components/computes/scriptloopcontext.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/computes/filternodecomputes.h>
#include <guicomponents/comms/message.h>
#include <guicomponents/quick/nodegraphmanipulator.h>

#include <QtQml/QQmlEngine>
#include <QtQml/QQmlExpression>
#include <QtQml/QQmlContext>

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>

#include <apps/smashbrowse/typescript/nodescripts/nodescripts.h>

namespace ngs {

FilterByTypeAndValueNodeCompute::FilterByTypeAndValueNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = filter_by_type_and_value(input.elements, element_type, target_value)\n";
}

FilterByTypeAndValueNodeCompute::~FilterByTypeAndValueNodeCompute() {
}

void FilterByTypeAndValueNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;
    create_input(Message::kElementType, c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = "";
    create_input(Message::kTargetValue, c);
  }
}

const QJsonObject FilterByTypeAndValueNodeCompute::_hints = FilterByTypeAndValueNodeCompute::init_hints();
QJsonObject FilterByTypeAndValueNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();

  add_hint(m, Message::kElementType, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::ElementType));
  add_hint(m, Message::kElementType, GUITypes::HintKey::DescriptionHint, "The type of element to match.");

  add_hint(m, Message::kTargetValue, GUITypes::HintKey::DescriptionHint, "The image or text value to match. Matches all non empty values if left blank.");
  return m;
}

// -------------------------------------------------------------------------------------------

FilterByPositionNodeCompute::FilterByPositionNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = filter_by_position(input.elements, global_mouse_position)";
}

FilterByPositionNodeCompute::~FilterByPositionNodeCompute() {
}

void FilterByPositionNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    QJsonObject pos;
    pos.insert("x", 0);
    pos.insert("y", 0);

    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = pos;

    create_input(Message::kGlobalMousePosition, c);
  }
}

const QJsonObject FilterByPositionNodeCompute::_hints = FilterByPositionNodeCompute::init_hints();
QJsonObject FilterByPositionNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  add_hint(m, Message::kGlobalMousePosition, GUITypes::HintKey::DescriptionHint, "The global mouse position in browser space.");
  return m;
}


// ----------------------------------------------------------------------------------------------------

FilterByDimensionsNodeCompute::FilterByDimensionsNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = filter_by_dimensions(input.elements, width, height, max_width_difference, max_height_difference)";
}

FilterByDimensionsNodeCompute::~FilterByDimensionsNodeCompute() {
}

void FilterByDimensionsNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 100;
    create_input(Message::kWidth, c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 100;
    create_input(Message::kHeight, c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 5;
    create_input(Message::kMaxWidthDifference, c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 5;
    create_input(Message::kMaxHeightDifference, c);
  }
}

const QJsonObject FilterByDimensionsNodeCompute::_hints = FilterByDimensionsNodeCompute::init_hints();
QJsonObject FilterByDimensionsNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();

  add_hint(m, Message::kWidth, GUITypes::HintKey::DescriptionHint, "The width of element to match.");
  add_hint(m, Message::kHeight, GUITypes::HintKey::DescriptionHint, "The height of element to match.");
  add_hint(m, Message::kMaxWidthDifference, GUITypes::HintKey::DescriptionHint, "The max width difference to match as a percentage of width.");
  add_hint(m, Message::kMaxHeightDifference, GUITypes::HintKey::DescriptionHint, "The max height difference to match as a percentage of height.");
  return m;
}

// ----------------------------------------------------------------------------------------------------

FilterByViewportNodeCompute::FilterByViewportNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = filter_by_viewport(input.elements)";
}

FilterByViewportNodeCompute::~FilterByViewportNodeCompute() {
}

void FilterByViewportNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
}

const QJsonObject FilterByViewportNodeCompute::_hints = FilterByViewportNodeCompute::init_hints();
QJsonObject FilterByViewportNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  return m;
}

// ----------------------------------------------------------------------------------------------------

FindClosestNodeCompute::FindClosestNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = sort_by_distance_to_anchors(input.elements, anchor_elements.elements)";
}

FindClosestNodeCompute::~FindClosestNodeCompute() {
}

void FindClosestNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = true;
    c.unconnected_value = QJsonObject();
    create_input(Message::kAnchorElements, c);
  }
}

const QJsonObject FindClosestNodeCompute::_hints = FindClosestNodeCompute::init_hints();
QJsonObject FindClosestNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  add_hint(m, Message::kAnchorElements, GUITypes::HintKey::DescriptionHint, "The anchoring elements from which distances will be measured.");
  return m;
}

// ----------------------------------------------------------------------------------------------------

FilterToSideMostNodeCompute::FilterToSideMostNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = find_sidemost(input.elements, direction)";
}

FilterToSideMostNodeCompute::~FilterToSideMostNodeCompute() {
}

void FilterToSideMostNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;
    create_input(Message::kDirection, c);
  }
}

const QJsonObject FilterToSideMostNodeCompute::_hints = FilterToSideMostNodeCompute::init_hints();
QJsonObject FilterToSideMostNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  add_hint(m, Message::kDirection, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::DirectionType));
  add_hint(m, Message::kDirection, GUITypes::HintKey::DescriptionHint, "The direction in which to scroll.");
  return m;
}

// ----------------------------------------------------------------------------------------------------

FilterByIndexNodeCompute::FilterByIndexNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = filter_by_index(input.elements, start_index, num_indices)";
}

FilterByIndexNodeCompute::~FilterByIndexNodeCompute() {
}

void FilterByIndexNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;
    create_input(Message::kStartIndex, c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 1;
    create_input(Message::kNumIndices, c);
  }
}

const QJsonObject FilterByIndexNodeCompute::_hints = FilterByIndexNodeCompute::init_hints();
QJsonObject FilterByIndexNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  add_hint(m, Message::kStartIndex, GUITypes::HintKey::DescriptionHint, "The starting index from which to extract elements.");
  add_hint(m, Message::kNumIndices, GUITypes::HintKey::DescriptionHint, "The total number of elements to extract.");
  return m;
}

// ----------------------------------------------------------------------------------------------------

JoinElementsNodeCompute::JoinElementsNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = join_elements(input.elements, other.elements)";
}

JoinElementsNodeCompute::~JoinElementsNodeCompute() {
}

void JoinElementsNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = true;
    c.unconnected_value = QJsonObject();
    create_input(Message::kOther, c);
  }
}

const QJsonObject JoinElementsNodeCompute::_hints = JoinElementsNodeCompute::init_hints();
QJsonObject JoinElementsNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  add_hint(m, Message::kOther, GUITypes::HintKey::DescriptionHint, "The elements to append to those in our main input.");
  return m;
}

}
