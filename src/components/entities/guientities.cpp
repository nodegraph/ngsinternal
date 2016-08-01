#include <components/entities/guientities.h>
#include <components/entities/nonguientities.h>

#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/entity.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

#include <components/interactions/shapecanvas.h>
#include <components/interactions/groupinteraction.h>

#include <components/resources/resources.h>

#include <components/compshapes/nodeshape.h>
#include <components/compshapes/compshapecollective.h>
#include <components/compshapes/dotnodeshape.h>
#include <components/compshapes/inputlabelshape.h>
#include <components/compshapes/inputnodeshape.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/outputlabelshape.h>
#include <components/compshapes/outputnodeshape.h>
#include <components/compshapes/nodegraphselection.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/groupnodeshape.h>

#include <components/computes/compute.h>
#include <components/computes/dotnodecompute.h>
#include <components/computes/groupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/mocknodecompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/outputnodecompute.h>
#include <components/computes/scriptnodecompute.h>
#include <components/computes/browsercomputes.h>

#include <components/entities/factory.h>
#include <components/interactions/graphbuilder.h>
#include <components/interactions/viewcontrols.h>
#include <gui/widget/nodegrapheditor.h>


/*
 * Entity Structure
 *
 * Syntax:  Entity : Components in the Entity
 * Indentation: Indented lines are child entities of the preceding line, otherwise they are siblings.
 * Dir has no components and acts a namespace
 *
 * Group:
 *   node_a : NodeShape, NodeCompute
 *     - inputs : dir
 *       - in_a : InputCompute, InputShape
 *         - label : InputLabelShape
 *     - outputs : dir
 *       - out_b : OutputCompute, OutputShape
 *         - label : OutputLabelShape
 *     - links : dir
 *       - link1 : LinkShape
 *   inputs : dir
 *   outputs : dir
 *   links : dir
 */

namespace ngs {

void QMLAppEntity::create_internals() {
  // Our components.
  new_ff Factory(this);
  // Gui related.
  new_ff GraphBuilder(this);
  new_ff Resources(this);
  new_ff NodeGraphSelection(this);
  new_ff ShapeCanvas(this);
  // Qt related. // Needs to be manually created due to dependency issues.
  //new_ff FBORenderer(this);
  //new_ff FBOWorker(this);
  //new_ff NodeGraphQuickItem(this);
}

void QtAppEntity::create_internals() {
  // Our components.
  new_ff Factory(this);
  // Gui related.
  new_ff GraphBuilder(this);
  new_ff Resources(this);
  new_ff NodeGraphSelection(this);
  new_ff ShapeCanvas(this);
  // Qt related.
  //new_ff NodeGraphEditor(this); // This needs to be manually created because it needs a valid Qt object to be it's parent, in order to render properly.
}

void AppEntity::create_internals() {
  // Our components.
  new_ff Factory(this);
  // Gui related.
  new_ff GraphBuilder(this);
  new_ff Resources(this);
  new_ff NodeGraphSelection(this);
  new_ff ShapeCanvas(this);
}

void GroupNodeEntity::create_internals() {
  // Our components.
  new_ff GroupNodeCompute(this);
  // Gui related.
  new_ff GroupInteraction(this);
  //new_ff GroupOverlayShape(this);
  new_ff CompShapeCollective(this);
  new_ff GroupNodeShape(this);

  // Our sub entities.
  {
    Entity* links = new_ff BaseNamespaceEntity(this, "links");
    links->create_internals();
  }
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
  }
}

void LinkEntity::create_internals() {
  // Our components.
  // Gui related.
  new_ff LinkShape(this);
}

void DotNodeEntity::create_internals() {
  // Our components.
  new_ff DotNodeCompute(this);
  // Gui related.
  DotNodeShape* shape = new_ff DotNodeShape(this);
  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
    Entity* input = new_ff InputEntity(inputs, "in");
    input->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
    Entity* output = new_ff OutputEntity(outputs,"out");
    output->create_internals();
  }
}

void InputNodeEntity::create_internals() {
  // Our components.
  new_ff InputNodeCompute(this);
  // Gui related.
  NodeShape* shape = new_ff InputNodeShape(this);
  shape->push_output_name("out");
  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
    Entity* output = new_ff OutputEntity(outputs, "out");
    output->create_internals();
  }
}

void OutputNodeEntity::create_internals() {
  // Our components.
  new_ff OutputNodeCompute(this);
  // Gui related.
  NodeShape* shape = new_ff OutputNodeShape(this);
  shape->push_input_name("in");
  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
    Entity* input = new_ff InputEntity(inputs, "in");
    input->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
  }
}

void MockNodeEntity::create_internals() {
  // Our components.
  new_ff MockNodeCompute(this);
  // Gui related.
  NodeShape* shape = new_ff NodeShape(this);
  shape->push_input_name("a");
  shape->push_input_name("b");
  shape->push_output_name("c");
  shape->push_output_name("d");
  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
    InputEntity* in_a = new_ff InputEntity(inputs, "a");
    in_a->create_internals();
    InputEntity* in_b = new_ff InputEntity(inputs, "b");
    in_b->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
    OutputEntity* out_c = new_ff OutputEntity(outputs, "c");
    out_c->create_internals();
    OutputEntity* out_d = new_ff OutputEntity(outputs, "d");
    out_d->create_internals();
  }
}

void OpenBrowserNodeEntity::create_internals() {
  // Our components.
  new_ff OpenBrowserCompute(this);
  // Gui related.
  NodeShape* shape = new_ff NodeShape(this);
  shape->push_input_name("in");
  shape->push_output_name("out");
  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
    InputEntity* in = new_ff InputEntity(inputs, "in");
    in->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
    OutputEntity* out = new_ff OutputEntity(outputs, "out");
    out->create_internals();
  }
}

void CloseBrowserNodeEntity::create_internals() {
  // Our components.
  new_ff CloseBrowserCompute(this);
  // Gui related.
  NodeShape* shape = new_ff NodeShape(this);
  shape->push_input_name("in");
  shape->push_output_name("out");
  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
    InputEntity* in = new_ff InputEntity(inputs, "in");
    in->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
    OutputEntity* out = new_ff OutputEntity(outputs, "out");
    out->create_internals();
  }
}

void CreateSetFromValuesNodeEntity::create_internals() {
  // Our components.
  new_ff CreateSetFromValuesCompute(this);
  // Gui related.
  NodeShape* shape = new_ff NodeShape(this);
  shape->push_input_name("in");
  shape->push_output_name("out");
  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
    InputEntity* in = new_ff InputEntity(inputs, "in");
    in->create_internals();
    BaseInputEntity* type = new_ff BaseInputEntity(inputs, "type");
    type->create_internals();
    BaseInputEntity* values = new_ff BaseInputEntity(inputs, "values");
    values->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
    OutputEntity* out = new_ff OutputEntity(outputs, "out");
    out->create_internals();
  }
}

void CreateSetFromTypeNodeEntity::create_internals() {
  // Our components.
  new_ff CreateSetFromTypeCompute(this);
  // Gui related.
  NodeShape* shape = new_ff NodeShape(this);
  shape->push_input_name("in");
  shape->push_output_name("out");
  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
    InputEntity* in = new_ff InputEntity(inputs, "in");
    in->create_internals();
    BaseInputEntity* type = new_ff BaseInputEntity(inputs, "type");
    type->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
    OutputEntity* out = new_ff OutputEntity(outputs, "out");
    out->create_internals();
  }
}


void ComputeNodeEntity::create_internals() {
  // Our components.
  Compute* dc = new_ff ScriptNodeCompute(this);
  // Gui related.
  NodeShape* shape = new_ff NodeShape(this);
  shape->push_input_name("in");
  shape->push_output_name("out");

  // Entity is fully build at this point, at least in terms of the non opengl gui components.
  // The data computer allows initializing at this point.
  // dc->update_deps();

  // Our sub entities.
  {
    Entity* inputs = new_ff BaseNamespaceEntity(this, "inputs");
    inputs->create_internals();
    InputEntity* in = new_ff InputEntity(inputs, "in");
    in->create_internals();
  }
  {
    Entity* outputs = new_ff BaseNamespaceEntity(this, "outputs");
    outputs->create_internals();
    OutputEntity* out = new_ff OutputEntity(outputs, "out");
    out->create_internals();
  }
}

void InputEntity::create_internals() {
  // Our components.
  new_ff InputCompute(this);
  // Gui related.
  new_ff InputShape(this);
  // Our sub entities.
  {
    Entity* label = new_ff InputLabelEntity(this, "label");
    label->create_internals();
  }
}

void InputLabelEntity::create_internals() {
  // Our components.
  // Gui related.
  new_ff InputLabelShape(this);
}

void OutputEntity::create_internals() {
  // Our components.
  new_ff OutputCompute(this);
  // Gui related.
  new_ff OutputShape(this);
  // Our sub entities.
  {
    Entity* label = new_ff OutputLabelEntity(this, "label");
    label->create_internals();
  }
}

void OutputLabelEntity::create_internals() {
  // Our components.
  // Gui related.
  new_ff OutputLabelShape(this);
}

}
