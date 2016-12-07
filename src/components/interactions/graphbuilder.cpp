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
#include <components/interactions/shapecanvas.h>

#include <guicomponents/comms/message.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

GraphBuilder::GraphBuilder(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _factory(this),
      _manipulator(this) {
  get_dep_loader()->register_fixed_dep(_factory, Path());
  get_dep_loader()->register_fixed_dep(_manipulator, Path());
}

GraphBuilder::~GraphBuilder() {
}

void GraphBuilder::build_stress_graph() {
  external();

  std::cerr << "starting to create nodes\n";

  Entity* root_group = get_entity(Path({"root"}));

  const size_t cols = 100;
  const size_t rows = 10;

  std::vector<Entity*> prev_row;
  prev_row.resize(cols);

  for (size_t x = 0; x < cols; ++x) {
    for (size_t y = 0; y < rows; ++y) {
      size_t num = y * cols + x;
      std::string name = boost::lexical_cast<std::string>(num);

      // Build a node.
      Entity* node = _manipulator->create_compute_node(true, EntityDID::kComputeNodeEntity, ComponentDID::kMockNodeCompute, name, root_group);

      // Position it.
      glm::vec2 pos(x * 300, y * 300);
      Dep<CompShape> cs = get_dep<CompShape>(node);
      cs->set_pos(pos);

      // Link up the node.
      if (y > 0) {
        // Connect links to the previous row.
        Dep<OutputCompute> op0 = get_dep<OutputCompute>(node->get_entity(Path({".","outputs","c"})));
        Dep<OutputCompute> op1 = get_dep<OutputCompute>(node->get_entity(Path({".","outputs","d"})));
        Dep<InputCompute> ip0 = get_dep<InputCompute>(prev_row[x]->get_entity(Path({".","inputs","a"})));
        Dep<InputCompute> ip1 = get_dep<InputCompute>(prev_row[x]->get_entity(Path({".","inputs","b"})));

        ip0->link_output_compute(op0);
        ip1->link_output_compute(op1);
      }
      prev_row[x] = node;
    }
  }

  std::cerr << "done creating nodes\n";
}

void GraphBuilder::build_test_graph() {
  external();

  Entity* root_group = get_entity(Path({"root"}));

  Entity* i1 = _manipulator->create_input_node(true, 1, "input one", root_group);
  Entity* o1 = _manipulator->create_node(true, EntityDID::kOutputNodeEntity, "output one", root_group);

  Entity* n1 = _manipulator->create_compute_node(true, EntityDID::kComputeNodeEntity, ComponentDID::kMockNodeCompute, "middle node", root_group);
  Entity* n2 = _manipulator->create_compute_node(true, EntityDID::kComputeNodeEntity, ComponentDID::kMockNodeCompute, "top node", root_group);
  Entity* n3 = _manipulator->create_compute_node(true, EntityDID::kComputeNodeEntity, ComponentDID::kMockNodeCompute, "bottom node", root_group);

  Entity* d1 = _manipulator->create_node(true, EntityDID::kDotNodeEntity, "dot1", root_group);
  Entity* d2 = _manipulator->create_node(true, EntityDID::kDotNodeEntity, "dot2", root_group);

  Entity* n1_ipe1 = n1->get_entity(Path({".","inputs","a"}));
  Entity* n1_ipe2 = n1->get_entity(Path({".","inputs","b"}));

  Entity* n2_ipe1 = n2->get_entity(Path({".","inputs","a"}));
  Entity* n2_ipe2 = n2->get_entity(Path({".","inputs","b"}));

  Entity* n3_ipe1 = n3->get_entity(Path({".","inputs","a"}));
  Entity* n3_ipe2 = n3->get_entity(Path({".","inputs","b"}));

  Entity* n1_ope1 = n1->get_entity(Path({".","outputs","c"}));
  Entity* n1_ope2 = n1->get_entity(Path({".","outputs","d"}));

  Entity* n2_ope1 = n2->get_entity(Path({".","outputs","c"}));
  Entity* n2_ope2 = n2->get_entity(Path({".","outputs","d"}));

  Entity* n3_ope1 = n3->get_entity(Path({".","outputs","c"}));
  Entity* n3_ope2 = n3->get_entity(Path({".","outputs","d"}));

  get_app_root()->initialize_wires();


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
    Entity* sub_group = _factory->instance_entity(root_group, EntityDID::kGroupNodeEntity, "group one");
    sub_group->create_internals();

    glm::vec2 pos;
    pos.x = 600;
    pos.y = 700;
    get_dep<CompShape>(sub_group)->set_pos(pos);

    Entity* n1 = _manipulator->create_compute_node(true, EntityDID::kComputeNodeEntity, ComponentDID::kMockNodeCompute, "sub middle node", sub_group);
    Entity* n2 = _manipulator->create_compute_node(true, EntityDID::kComputeNodeEntity, ComponentDID::kMockNodeCompute, "sub top node", sub_group);
    Entity* n3 = _manipulator->create_compute_node(true, EntityDID::kComputeNodeEntity, ComponentDID::kMockNodeCompute, "sub bottom node", sub_group);
    Entity* n4 = _manipulator->create_compute_node(true, EntityDID::kComputeNodeEntity, ComponentDID::kScriptNodeCompute, "NoOp", sub_group);
    Entity* i1 = _manipulator->create_input_node(true, 1, "input1", sub_group);
    Entity* o1 = _manipulator->create_node(true, EntityDID::kOutputNodeEntity, "output1", sub_group);
    o1->create_internals();

    Entity* n1_ipe1 = n1->get_entity(Path({".","inputs","a"}));
    Entity* n1_ipe2 = n1->get_entity(Path({".","inputs","b"}));

    Entity* n2_ipe1 = n2->get_entity(Path({".","inputs","a"}));
    Entity* n2_ipe2 = n2->get_entity(Path({".","inputs","b"}));

    Entity* n3_ipe1 = n3->get_entity(Path({".","inputs","a"}));
    Entity* n3_ipe2 = n3->get_entity(Path({".","inputs","b"}));

    Entity* n1_ope1 = n1->get_entity(Path({".","outputs","c"}));
    Entity* n1_ope2 = n1->get_entity(Path({".","outputs","d"}));

    Entity* n2_ope1 = n2->get_entity(Path({".","outputs","c"}));
    Entity* n2_ope2 = n2->get_entity(Path({".","outputs","d"}));

    Entity* n3_ope1 = n3->get_entity(Path({".","outputs","c"}));
    Entity* n3_ope2 = n3->get_entity(Path({".","outputs","d"}));

    get_app_root()->initialize_wires();

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
