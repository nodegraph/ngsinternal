#include <base/objectmodel/basefactory.h>
#include <base/objectmodel/deploader.h>

// For building test graphs.
#include <entities/guientities.h>
#include <entities/nonguientities.h>
#include <components/compshapes/compshape.h>
#include <entities/factory.h>
#include <boost/lexical_cast.hpp>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/interactions/graphbuilder.h>


namespace ngs {

GraphBuilder::GraphBuilder(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _factory(this) {
  get_dep_loader()->register_fixed_dep(_factory, "");
}

GraphBuilder::~GraphBuilder() {
}

Entity* GraphBuilder::build_open_browser_node() {
  BaseEntityInstancer* ei = _factory->get_entity_instancer();
  Entity* root_group = get_entity("root");

  Entity* node = ei->instance(root_group, "open browser", kOpenBrowserNodeEntity);
  node->create_internals();

  // Todo: Connect it to the last selected node. Then make this node selected.

  return node;
}

Entity* GraphBuilder::build_close_browser_node() {
  BaseEntityInstancer* ei = _factory->get_entity_instancer();
  Entity* root_group = get_entity("root");

  Entity* node = ei->instance(root_group, "close browser", kCloseBrowserNodeEntity);
  node->create_internals();

  // Todo: Connect it to the last selected node. Then make this node selected.

  return node;
}

Entity* GraphBuilder::build_click_node(int set_index, int overlay_index, float rel_x, float rel_y) {
  BaseEntityInstancer* ei = _factory->get_entity_instancer();
  Entity* root_group = get_entity("root");

  Entity* node = ei->instance(root_group, "input one", kInputNodeEntity);
  node->create_internals();

  Dep<InputCompute> si_dep = get_dep<InputCompute>(node->get_entity("./inputs/set_index"));
  Dep<InputCompute> oi_dep = get_dep<InputCompute>(node->get_entity("./inputs/overlay_index"));
  Dep<InputCompute> x_dep = get_dep<InputCompute>(node->get_entity("./inputs/x"));
  Dep<InputCompute> y_dep = get_dep<InputCompute>(node->get_entity("./inputs/y"));

  si_dep->set_value(set_index);
  oi_dep->set_value(overlay_index);
  x_dep->set_value(rel_x);
  y_dep->set_value(rel_y);

  // Todo: Connect it to the last selected node. Then make this node selected.

  return node;
}

void GraphBuilder::build_stress_graph() {

  std::cerr << "starting to create nodes\n";

  BaseEntityInstancer* ei = _factory->get_entity_instancer();

  Entity* root_group = get_entity("root");

  const size_t cols = 100;
  const size_t rows = 10;

  std::vector<Entity*> prev_row;
  prev_row.resize(cols);

  for (size_t x = 0; x < cols; ++x) {
    for (size_t y = 0; y < rows; ++y) {
      size_t num = y * cols + x;
      std::string name = boost::lexical_cast<std::string>(num);

      // Build a node.
      Entity* node = ei->instance(root_group, name, kMockNodeEntity);
      node->create_internals();

      // Position it.
      glm::vec2 pos(x * 300, y * 300);
      Dep<CompShape> cs = get_dep<CompShape>(node);
      cs->set_pos(pos);

      // Link up the node.
      if (y > 0) {
        // Connect links to the previous row.
        Dep<OutputCompute> op0 = get_dep<OutputCompute>(node->get_entity("./outputs/c"));
        Dep<OutputCompute> op1 = get_dep<OutputCompute>(node->get_entity("./outputs/d"));
        Dep<InputCompute> ip0 = get_dep<InputCompute>(prev_row[x]->get_entity("./inputs/a"));
        Dep<InputCompute> ip1 = get_dep<InputCompute>(prev_row[x]->get_entity("./inputs/b"));

        ip0->link_output_compute(op0);
        ip1->link_output_compute(op1);
      }
      prev_row[x] = node;
    }
  }

  std::cerr << "done creating nodes\n";
}

void GraphBuilder::build_test_graph() {
  BaseEntityInstancer* ei = _factory->get_entity_instancer();
  Entity* root_group = get_entity("root");

  Entity* i1 = ei->instance(root_group, "input one", kInputNodeEntity);
  Entity* o1 = ei->instance(root_group, "output one", kOutputNodeEntity);
  i1->create_internals();
  o1->create_internals();

  Entity* n1 = ei->instance(root_group, "middle node", kMockNodeEntity);
  Entity* n2 = ei->instance(root_group, "top node", kMockNodeEntity);
  Entity* n3 = ei->instance(root_group, "bottom node", kMockNodeEntity);
  n1->create_internals();
  n2->create_internals();
  n3->create_internals();

  Entity* d1 = ei->instance(root_group, "dot1", kDotNodeEntity);
  Entity* d2 = ei->instance(root_group, "dot2", kDotNodeEntity);
  d1->create_internals();
  d2->create_internals();

  Entity* n1_ipe1 = n1->get_entity("./inputs/a");
  Entity* n1_ipe2 = n1->get_entity("./inputs/b");

  Entity* n2_ipe1 = n2->get_entity("./inputs/a");
  Entity* n2_ipe2 = n2->get_entity("./inputs/b");

  Entity* n3_ipe1 = n3->get_entity("./inputs/a");
  Entity* n3_ipe2 = n3->get_entity("./inputs/b");

  Entity* n1_ope1 = n1->get_entity("./outputs/c");
  Entity* n1_ope2 = n1->get_entity("./outputs/d");

  Entity* n2_ope1 = n2->get_entity("./outputs/c");
  Entity* n2_ope2 = n2->get_entity("./outputs/d");

  Entity* n3_ope1 = n3->get_entity("./outputs/c");
  Entity* n3_ope2 = n3->get_entity("./outputs/d");

  get_app_root()->initialize_deps();
  get_app_root()->update_deps_and_hierarchy();


  Dep<InputCompute> n1_ip1 = get_dep<InputCompute>(n1_ipe1);
  Dep<InputCompute> n1_ip2 = get_dep<InputCompute>(n1_ipe2);
  Dep<OutputCompute> n2_op1 = get_dep<OutputCompute>(n2_ope1);
  Dep<OutputCompute> n2_op2 = get_dep<OutputCompute>(n2_ope2);
  Dep<InputCompute> n3_ip1 = get_dep<InputCompute>(n3_ipe1);
  Dep<InputCompute> n3_ip2 = get_dep<InputCompute>(n3_ipe2);

//  n1_ip1->connect(n2_op1);
//  n1_ip2->connect(n2_op2);
//
//  n3_ip1->connect(n2_op1);
//  n3_ip2->connect(n2_op2);

  glm::vec2 pos;
  pos.x = 300;
  pos.y = 900;
  get_dep<CompShape>(i1)->set_pos(pos);

  pos.x = 500;
  pos.y = 900;
  get_dep<CompShape>(o1)->set_pos(pos);

  pos.x = 300;
  pos.y = 390;
  get_dep<CompShape>(n1)->set_pos(pos);

  pos.x = 500;
  pos.y = 390;
  get_dep<CompShape>(n1)->set_pos(pos);

  pos.x = 330;
  pos.y = 600;
  get_dep<CompShape>(n2)->set_pos(pos);

  pos.x = 300;
  pos.y = 140;
  get_dep<CompShape>(n3)->set_pos(pos);

  pos.x = 600;
  pos.y = 800;
  get_dep<CompShape>(d1)->set_pos(pos);

  pos.x = 750;
  pos.y = 800;
  get_dep<CompShape>(d2)->set_pos(pos);

  // ------------------------------------------------------------------------
  // Create a sub group.
  // ------------------------------------------------------------------------

  {
    Entity* sub_group = ei->instance(root_group, "group one", kGroupNodeEntity);
    sub_group->create_internals();

    glm::vec2 pos;
    pos.x = 600;
    pos.y = 700;
    get_dep<CompShape>(sub_group)->set_pos(pos);

    Entity* n1 = ei->instance(sub_group, "sub middle node", kMockNodeEntity);
    Entity* n2 = ei->instance(sub_group, "sub top node", kMockNodeEntity);
    Entity* n3 = ei->instance(sub_group, "sub bottom node", kMockNodeEntity);
    Entity* n4 = ei->instance(sub_group, "NoOp", kScriptNodeEntity);

    n1->create_internals();
    n2->create_internals();
    n3->create_internals();
    n4->create_internals();

    Entity* i1 = ei->instance(sub_group, "input1", kInputNodeEntity);
    Entity* o1 = ei->instance(sub_group, "output1", kOutputNodeEntity);

    i1->create_internals();
    o1->create_internals();

    Entity* n1_ipe1 = n1->get_entity("./inputs/a");
    Entity* n1_ipe2 = n1->get_entity("./inputs/b");

    Entity* n2_ipe1 = n2->get_entity("./inputs/a");
    Entity* n2_ipe2 = n2->get_entity("./inputs/b");

    Entity* n3_ipe1 = n3->get_entity("./inputs/a");
    Entity* n3_ipe2 = n3->get_entity("./inputs/b");

    Entity* n1_ope1 = n1->get_entity("./outputs/c");
    Entity* n1_ope2 = n1->get_entity("./outputs/d");

    Entity* n2_ope1 = n2->get_entity("./outputs/c");
    Entity* n2_ope2 = n2->get_entity("./outputs/d");

    Entity* n3_ope1 = n3->get_entity("./outputs/c");
    Entity* n3_ope2 = n3->get_entity("./outputs/d");

    get_app_root()->initialize_deps();
    get_app_root()->update_deps_and_hierarchy();

	Dep<InputCompute> n1_ip1 = get_dep<InputCompute>(n1_ipe1);
	Dep<InputCompute> n1_ip2 = get_dep<InputCompute>(n1_ipe2);
	Dep<OutputCompute> n2_op1 = get_dep<OutputCompute>(n2_ope1);
	Dep<OutputCompute> n2_op2 = get_dep<OutputCompute>(n2_ope2);
	Dep<InputCompute> n3_ip1 = get_dep<InputCompute>(n3_ipe1);
	Dep<InputCompute> n3_ip2 = get_dep<InputCompute>(n3_ipe2);

    n1_ip1->link_output_compute(n2_op1);
    n1_ip2->link_output_compute(n2_op2);

    n3_ip1->link_output_compute(n2_op1);
    n3_ip2->link_output_compute(n2_op2);

    //glm::vec2 pos;
    pos.x = 300;
    pos.y = 390;
    get_dep<CompShape>(n1)->set_pos(pos);

    pos.x = 500;
    pos.y = 390;
    get_dep<CompShape>(n1)->set_pos(pos);

    pos.x = 330;
    pos.y = 600;
    get_dep<CompShape>(n2)->set_pos(pos);

    pos.x = 300;
    pos.y = 140;
    get_dep<CompShape>(n3)->set_pos(pos);

    pos.x = 400;
    pos.y = 250;
    get_dep<CompShape>(n4)->set_pos(pos);

    pos.x = 600;
    pos.y = 700;
    get_dep<CompShape>(i1)->set_pos(pos);

    pos.x = 600;
    pos.y = 600;
    get_dep<CompShape>(o1)->set_pos(pos);

  }
}



}
