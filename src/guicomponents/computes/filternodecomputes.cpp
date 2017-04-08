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

#include <apps/youmacro/typescript/nodescripts/nodescripts.h>

namespace ngs {

IsolateByTypeAndValueNodeCompute::IsolateByTypeAndValueNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = isolate_by_type_and_value(input.elements, element_type, target_value)\n";
}

IsolateByTypeAndValueNodeCompute::~IsolateByTypeAndValueNodeCompute() {
}

void IsolateByTypeAndValueNodeCompute::create_inputs_outputs(const EntityConfig& config) {
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

const QJsonObject IsolateByTypeAndValueNodeCompute::_hints = IsolateByTypeAndValueNodeCompute::init_hints();
QJsonObject IsolateByTypeAndValueNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();

  add_hint(m, Message::kElementType, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::ElementType));
  add_hint(m, Message::kElementType, GUITypes::HintKey::DescriptionHint, "The type of element to match.");

  add_hint(m, Message::kTargetValue, GUITypes::HintKey::DescriptionHint, "The image or text value to match. Matches all non empty values if left blank.");
  return m;
}

// -------------------------------------------------------------------------------------------

IsolateByPositionNodeCompute::IsolateByPositionNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = isolate_by_position(input.elements, global_mouse_position)";
}

IsolateByPositionNodeCompute::~IsolateByPositionNodeCompute() {
}

void IsolateByPositionNodeCompute::create_inputs_outputs(const EntityConfig& config) {
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

const QJsonObject IsolateByPositionNodeCompute::_hints = IsolateByPositionNodeCompute::init_hints();
QJsonObject IsolateByPositionNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  add_hint(m, Message::kGlobalMousePosition, GUITypes::HintKey::DescriptionHint, "The global mouse position in browser space.");
  return m;
}


// ----------------------------------------------------------------------------------------------------

IsolateByDimensionsNodeCompute::IsolateByDimensionsNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = isolate_by_dimensions(input.elements, width, height, max_width_difference, max_height_difference)";
}

IsolateByDimensionsNodeCompute::~IsolateByDimensionsNodeCompute() {
}

void IsolateByDimensionsNodeCompute::create_inputs_outputs(const EntityConfig& config) {
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

const QJsonObject IsolateByDimensionsNodeCompute::_hints = IsolateByDimensionsNodeCompute::init_hints();
QJsonObject IsolateByDimensionsNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();

  add_hint(m, Message::kWidth, GUITypes::HintKey::DescriptionHint, "The width of element to match.");
  add_hint(m, Message::kHeight, GUITypes::HintKey::DescriptionHint, "The height of element to match.");
  add_hint(m, Message::kMaxWidthDifference, GUITypes::HintKey::DescriptionHint, "The max width difference to match as a percentage of width.");
  add_hint(m, Message::kMaxHeightDifference, GUITypes::HintKey::DescriptionHint, "The max height difference to match as a percentage of height.");
  return m;
}

// ----------------------------------------------------------------------------------------------------

IsolateByViewportNodeCompute::IsolateByViewportNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = isolate_by_viewport(input.elements)";
}

IsolateByViewportNodeCompute::~IsolateByViewportNodeCompute() {
}

void IsolateByViewportNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
}

const QJsonObject IsolateByViewportNodeCompute::_hints = IsolateByViewportNodeCompute::init_hints();
QJsonObject IsolateByViewportNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  return m;
}

// ----------------------------------------------------------------------------------------------------

SortByDistanceToAnchorsNodeCompute::SortByDistanceToAnchorsNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = sort_by_distance_to_anchors(input.elements, anchors.elements)";
}

SortByDistanceToAnchorsNodeCompute::~SortByDistanceToAnchorsNodeCompute() {
}

void SortByDistanceToAnchorsNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = true;
    c.unconnected_value = QJsonObject();
    create_input(Message::kAnchors, c);
  }
}

const QJsonObject SortByDistanceToAnchorsNodeCompute::_hints = SortByDistanceToAnchorsNodeCompute::init_hints();
QJsonObject SortByDistanceToAnchorsNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  add_hint(m, Message::kAnchors, GUITypes::HintKey::DescriptionHint, "The anchoring elements from which distances will be measured.");
  return m;
}

// ----------------------------------------------------------------------------------------------------

IsolateByBoundaryNodeCompute::IsolateByBoundaryNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = isolate_by_boundary(input.elements, direction)";
}

IsolateByBoundaryNodeCompute::~IsolateByBoundaryNodeCompute() {
}

void IsolateByBoundaryNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;
    create_input(Message::kDirection, c);
  }
}

const QJsonObject IsolateByBoundaryNodeCompute::_hints = IsolateByBoundaryNodeCompute::init_hints();
QJsonObject IsolateByBoundaryNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  add_hint(m, Message::kDirection, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::DirectionType));
  add_hint(m, Message::kDirection, GUITypes::HintKey::DescriptionHint, "The direction in which to scroll.");
  return m;
}

// ----------------------------------------------------------------------------------------------------

IsolateToOneSideNodeCompute::IsolateToOneSideNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = isolate_to_one_side(input.elements, anchors.elements, direction)";
}

IsolateToOneSideNodeCompute::~IsolateToOneSideNodeCompute() {
}

void IsolateToOneSideNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = true;
    c.unconnected_value = QJsonObject();
    create_input(Message::kAnchors, c);
  }
  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 0;
    create_input(Message::kDirection, c);
  }
}

const QJsonObject IsolateToOneSideNodeCompute::_hints = IsolateToOneSideNodeCompute::init_hints();
QJsonObject IsolateToOneSideNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();

  add_hint(m, Message::kAnchors, GUITypes::HintKey::DescriptionHint, "Elements will be isolated to one side of these anchor elements.");

  add_hint(m, Message::kDirection, GUITypes::HintKey::EnumHint, to_underlying(GUITypes::EnumHintValue::DirectionType));
  add_hint(m, Message::kDirection, GUITypes::HintKey::DescriptionHint, "The side around the anchor elements in which to isolate.");
  return m;
}

// ----------------------------------------------------------------------------------------------------

IsolateByIndexNodeCompute::IsolateByIndexNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = isolate_by_index(input.elements, start_index, num_indices)";
}

IsolateByIndexNodeCompute::~IsolateByIndexNodeCompute() {
}

void IsolateByIndexNodeCompute::create_inputs_outputs(const EntityConfig& config) {
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

const QJsonObject IsolateByIndexNodeCompute::_hints = IsolateByIndexNodeCompute::init_hints();
QJsonObject IsolateByIndexNodeCompute::init_hints() {
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

// ----------------------------------------------------------------------------------------------------

PruneElementsNodeCompute::PruneElementsNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = prune_elements(input.elements, prune.elements)";
}

PruneElementsNodeCompute::~PruneElementsNodeCompute() {
}

void PruneElementsNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  BaseScriptNodeCompute::create_inputs_outputs(config);
  {
    EntityConfig c = config;
    c.expose_plug = true;
    c.unconnected_value = QJsonObject();
    create_input(Message::kPrune, c);
  }
}

const QJsonObject PruneElementsNodeCompute::_hints = PruneElementsNodeCompute::init_hints();
QJsonObject PruneElementsNodeCompute::init_hints() {
  QJsonObject m = BaseScriptNodeCompute::init_hints();
  add_hint(m, Message::kPrune, GUITypes::HintKey::DescriptionHint, "The elements to prune from those in our main input.");
  return m;
}

// ----------------------------------------------------------------------------------------------------

PruneDuplicatesNodeCompute::PruneDuplicatesNodeCompute(Entity* entity):
  BaseScriptNodeCompute(entity, kDID()) {
  _script_body = QString::fromUtf8((const char*)node_scripts, node_scripts_length);
  _script_body += "output.elements = prune_duplicates(input.elements)";
}

PruneDuplicatesNodeCompute::~PruneDuplicatesNodeCompute() {
}

}
