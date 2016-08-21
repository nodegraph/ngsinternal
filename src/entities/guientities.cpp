#include <entities/guientities.h>
#include <entities/nonguientities.h>
#include <entities/factory.h>

#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/component.h>
#include <base/objectmodel/entity.h>

#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>

#include <components/interactions/shapecanvas.h>
#include <components/interactions/groupinteraction.h>
#include <components/interactions/graphbuilder.h>
#include <components/interactions/viewcontrols.h>

#include <components/resources/resources.h>

#include <components/compshapes/compshapecollective.h>
#include <components/compshapes/dotnodeshape.h>
#include <components/compshapes/inputlabelshape.h>
#include <components/compshapes/inputnodeshape.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/outputlabelshape.h>
#include <components/compshapes/outputnodeshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/groupnodeshape.h>
#include <components/compshapes/nodeselection.h>
#include <components/compshapes/rectnodeshape.h>

#include <components/computes/compute.h>
#include <components/computes/dotnodecompute.h>
#include <components/computes/groupnodecompute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/inputnodecompute.h>
#include <components/computes/mocknodecompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/outputnodecompute.h>

#include <gui/widget/nodegrapheditor.h>

#include <guicomponents/comms/appcomm.h>
#include <guicomponents/comms/licensechecker.h>
#include <guicomponents/comms/filemodel.h>
#include <guicomponents/computes/browsercomputes.h>
#include <guicomponents/computes/scriptnodecompute.h>
#include <guicomponents/quick/fborenderer.h>
#include <guicomponents/quick/fboworker.h>
#include <guicomponents/quick/nodegraphquickitem.h>


/*
 * Entity Structure
 *
 * Syntax:  Entity : Components in the Entity
 * Indentation: Indented lines are child entities of the preceding line, otherwise they are siblings.
 * Dir has no components and acts a namespace
 *
 * Group:
 *   node_a : RectNodeShape, NodeCompute
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
  new_ff NodeSelection(this);
  new_ff ShapeCanvas(this);
  // Node Graph Rendering through QtQuick/QML.
  new_ff FBORenderer(this);
  new_ff FBOWorker(this);
  new_ff NodeGraphQuickItem(this);
  // Qt Releated Components.
  new_ff FileModel(this);
  new_ff AppComm(this);
  new_ff LicenseChecker(this);
}

FileModel* QMLAppEntity::get_file_model() {
  return get<FileModel>();
}

AppComm* QMLAppEntity::get_app_comm() {
  return get<AppComm>();
}

LicenseChecker* QMLAppEntity::get_license_checker() {
  return get<LicenseChecker>();
}

NodeGraphQuickItem* QMLAppEntity::get_node_graph_quick_item() {
  return get<NodeGraphQuickItem>();
}

void QtAppEntity::create_internals() {
  // Our components.
  new_ff Factory(this);
  // Gui related.
  new_ff GraphBuilder(this);
  new_ff Resources(this);
  new_ff NodeSelection(this);
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
  new_ff NodeSelection(this);
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

  // Create our internal namespace entities.
  create_namespaces();
}

void GroupNodeEntity::create_namespaces() {
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

void GroupNodeEntity::copy(SimpleSaver& saver, const std::unordered_set<Entity*>& children) const {
    if (children.empty()) {
      return;
    }

    // Find the relevant links to save.
    std::unordered_set<Entity*> links_to_save;
    Entity* links_folder = get_child("links");
    if (links_folder) {
      const NameToChildMap &links = links_folder->get_children();
      NameToChildMap::const_iterator iter;
      for (iter = links.begin(); iter != links.end(); ++iter) {
        LinkShape* link_shape = static_cast<LinkShape*>(iter->second->get(LinkShape::kIID()));
        assert(link_shape);

        const Dep<InputShape>& input_shape = link_shape->get_input_shape();
        const Dep<NodeShape>& input_node_shape = input_shape->get_node_shape();
        Entity* input_node_entity = input_node_shape->our_entity();
        assert(input_node_entity);

        const Dep<OutputShape>& output_shape = link_shape->get_output_shape();
        const Dep<NodeShape>& output_node_shape = output_shape->get_node_shape();
        Entity* output_node_entity = output_node_shape->our_entity();
        assert(output_node_entity);

        if (children.count(input_node_entity) && children.count(output_node_entity)) {
          links_to_save.insert(iter->second);
        }
      }
    }

    // Save the number of entities.
    size_t num_nodes = children.size();
    if (links_to_save.size()) {
      // +1 for the links folder.
      num_nodes += 1;
    }
    saver.save(num_nodes);

    // Save the entities.
    for (Entity* e : children) {
      e->save(saver);
    }

    // Save the links folder.
    if (links_to_save.size()) {
      // Save out info about the folder.
      links_folder->pre_save(saver);
      links_folder->save_components(saver);
      // Save out the contents of the folder.
      size_t num_links = links_to_save.size();
      saver.save(num_links);
      for (Entity* e: links_to_save) {
        e->save(saver);
      }
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
  RectNodeShape* shape = new_ff InputNodeShape(this);
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
  RectNodeShape* shape = new_ff OutputNodeShape(this);
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
  RectNodeShape* shape = new_ff RectNodeShape(this);
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
  RectNodeShape* shape = new_ff RectNodeShape(this);
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
  RectNodeShape* shape = new_ff RectNodeShape(this);
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
  RectNodeShape* shape = new_ff RectNodeShape(this);
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
  RectNodeShape* shape = new_ff RectNodeShape(this);
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
  RectNodeShape* shape = new_ff RectNodeShape(this);
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
