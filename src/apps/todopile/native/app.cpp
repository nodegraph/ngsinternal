//#include <base/device/transforms/glmhelper.h>
//#include <native/app.h>
//#include <base/memoryallocator/taggednew.h>
//
//#include <base/memorytracker/memorytracker.h>
//#include <base/objectmodel/entity.h>
//
//#include <components/interactions/shapecanvas.h>
//#include <base/utils/fileutil.h>
//#include <components/compshapes/compshapecollective.h>
//#include <components/interactions/groupinteraction.h>
//#include <components/interactions/nodegraphstate.h>
//#include <components/interactions/nodegroup.h>
//#include <gui/qt/qml/nodegraphquickitem.h>
//#include <gui/qt/qml/nodegraphquickitemglobals.h>
//
//// System.
//#include <iostream>
//#include <string>
//
//// Qt.
//#include <QtCore/QThread>
//#include <QtGui/QGuiApplication>
//#include <QtWidgets/QApplication>
//#include <QtQml/QQmlApplicationEngine>
//#include <QtQml/QQmlContext>
//#include <QtQuick/QQuickView>
//
//namespace open_node_graph {
//
//App::App()
//{
//}
//
//Entity* App::create_ng_root() {
//  // Set this on a global variable which is used by our QML c++ widgets.
//  g_ng_helper = GuiFactory::build_app2();
//  g_ng_root = *(g_ng_helper->get_children().begin());
//  return g_ng_root;
//}
//
//void App::load_demo(Entity* ng_root) {
//  // Clear node graph state.
//  NodeGraphState* ng_state = ng_root->get_app_entity()->get<NodeGraphState>();
//  ng_state->clear_all();
//
//  // Clear existing nodes. NodeGraphEditors will listen and switch to the root group.
//  NodeGroup* group = ng_root->get<NodeGroup>();
//  group->clear();
//
//  // Add our test nodes.
//  add_test_nodes(ng_root);
//
//  // Tessellate everything in the group.
//  group->switch_to<CompShapeCollective>()->update_state();
//}
//
//
//void App::add_test_nodes(Entity* ng_root) {
//
//    open_node_graph::NetworkFactory::build_test_graph(ng_root);
//
////  using namespace open_node_graph;
////
//////    Group* group = ng_root->get_ng_state()->get_current_group();
//////    assert(group == ng_root->get_group());
////
////    NodeGroup* group = ng_root->get<NodeGroup>();
////    LinkGroup* link_group = ng_root->get_link_group();
////
////    Node* n1 = group->build_node(kMockNodeEntity,"middle node")->get_node(); //hello there
////    Node* n2 = group->build_node(kMockNodeEntity,"top node")->get_node();
////    Node* n3 = group->build_node(kMockNodeEntity,"bottom node")->get_node();
////
////    Node* d1 = group->build_node(kDotNodeEntity, "dot1")->get_node(); //group->create_dot_node("dot1");
////    Node* d2 = group->build_node(kDotNodeEntity, "dot2")->get_node();//group->create_dot_node("dot2");
////
////    Link* link1 = link_group->create_link()->get_link();
////    Link* link2 = link_group->create_link()->get_link();
////    Link* link3 = link_group->create_link()->get_link();
////    Link* link4 = link_group->create_link()->get_link();
////
////    n1->remove_input_plug_by_index((size_t)0);
////    n1->remove_input_plug_by_index((size_t)0);
////
////    n2->remove_input_plug_by_index((size_t)0);
////    n2->remove_input_plug_by_index((size_t)0);
////
////    n3->remove_input_plug_by_index((size_t)0);
////    n3->remove_input_plug_by_index((size_t)0);
////
////    n1->remove_output_plug_by_index((size_t)0);
////    n1->remove_output_plug_by_index((size_t)0);
////
////    n2->remove_output_plug_by_index((size_t)0);
////    n2->remove_output_plug_by_index((size_t)0);
////
////    n3->remove_output_plug_by_index((size_t)0);
////    n3->remove_output_plug_by_index((size_t)0);
////
////    n1->create_input_plug("bacon",kQVariant);
////    n1->create_input_plug("candy",kQVariant);
////
////    n2->create_input_plug("sugar",kQVariant);
////    n2->create_input_plug("salt",kQVariant);
////
////    n3->create_input_plug("chewy",kQVariant);
////    n3->create_input_plug("gummy",kQVariant);
////
////    n1->create_output_plug("hot",kQVariant);
////    n1->create_output_plug("spicy",kQVariant);
////
////    n2->create_output_plug("jello",kQVariant);
////    n2->create_output_plug("fudge",kQVariant);
////
////    n3->create_output_plug("glazed",kQVariant);
////    n3->create_output_plug("sprinkles",kQVariant);
////
////
////    link1->connect_plugs(n1->get_input_plug(0),n2->get_output_plug(0));
////    link2->connect_plugs(n1->get_input_plug(1),n2->get_output_plug(1));
////
////    link3->connect_plugs(n3->get_input_plug(0),n2->get_output_plug(0));
////    link4->connect_plugs(n3->get_input_plug(1),n2->get_output_plug(0));
////
////  //  n1->get_output_plug(0)->connect(n3->get_input_plug(0));
////  //  n1->get_output_plug(1)->connect(n3->get_input_plug(1));
////
////    glm::vec2 pos;
////    pos.x = 300;
////    pos.y = 390;
////    n1->switch_to_node_properties()->set_anchor_pos(pos);
////
////    pos.x = 500;
////    pos.y = 390;
////    n1->switch_to_node_properties()->set_anchor_pos(pos);
////
////    pos.x = 330;
////    pos.y = 600;
////    n2->switch_to_node_properties()->set_anchor_pos(pos);
////
////    pos.x = 300;
////    pos.y = 140;
////    n3->switch_to_node_properties()->set_anchor_pos(pos);
////
////    pos.x = 600;
////    pos.y = 800;
////    d1->switch_to_node_properties()->set_anchor_pos(pos);
////
////    pos.x = 750;
////    pos.y = 800;
////    d2->switch_to_node_properties()->set_anchor_pos(pos);
////
////
////    // ------------------------------------------------------------------------
////    // Create a sub group.
////    // ------------------------------------------------------------------------
////
////    {
////      Node* g1 = group->build_node(kGroupNodeEntity,"group one")->get_node(); //group->create_group_node("group one");
////
////
////      glm::vec2 pos;
////      pos.x = 600;
////      pos.y = 700;
////      g1->switch_to_node_properties()->set_anchor_pos(pos);
////
////      NodeGroup* group2 = g1->switch_to_node_group();
////      LinkGroup* link_group2 = g1->switch_to_link_group();
////
////      Node* n1 = group2->build_node(kMockNodeEntity,"sub middle node")->get_node(); //hello there
////      Node* n2 = group2->build_node(kMockNodeEntity,"sub top node")->get_node();
////      Node* n3 = group2->build_node(kMockNodeEntity,"sub bottom node")->get_node();
////      Node* n4 = group2->build_compute_node("NoOp","YAHAH")->get_node();
////
////      Link* link1 = link_group2->create_link()->get_link();
////      Link* link2 = link_group2->create_link()->get_link();
////      Link* link3 = link_group2->create_link()->get_link();
////      Link* link4 = link_group2->create_link()->get_link();
////
////      Node* i1 = group2->build_node(kInputNodeEntity,"input1")->get_node(); //group2->create_input_node("input1");
////      Node* o1 = group2->build_node(kOutputNodeEntity,"output1")->get_node(); //group2->create_output_node("output1");
////
////      n1->remove_input_plug_by_index((size_t)0);
////      n1->remove_input_plug_by_index((size_t)0);
////
////      n2->remove_input_plug_by_index((size_t)0);
////      n2->remove_input_plug_by_index((size_t)0);
////
////      n3->remove_input_plug_by_index((size_t)0);
////      n3->remove_input_plug_by_index((size_t)0);
////
////      n1->remove_output_plug_by_index((size_t)0);
////      n1->remove_output_plug_by_index((size_t)0);
////
////      n2->remove_output_plug_by_index((size_t)0);
////      n2->remove_output_plug_by_index((size_t)0);
////
////      n3->remove_output_plug_by_index((size_t)0);
////      n3->remove_output_plug_by_index((size_t)0);
////
////      n1->create_input_plug("bacon",kQVariant);
////      n1->create_input_plug("candy",kQVariant);
////
////      n2->create_input_plug("sugar",kQVariant);
////      n2->create_input_plug("salt",kQVariant);
////
////      n3->create_input_plug("chewy",kQVariant);
////      n3->create_input_plug("gummy",kQVariant);
////
////      n1->create_output_plug("hot",kQVariant);
////      n1->create_output_plug("spicy",kQVariant);
////
////      n2->create_output_plug("jello",kQVariant);
////      n2->create_output_plug("fudge",kQVariant);
////
////      n3->create_output_plug("glazed",kQVariant);
////      n3->create_output_plug("sprinkles",kQVariant);
////
////
////      link1->connect_plugs(n1->get_input_plug(0),n2->get_output_plug(0));
////      link2->connect_plugs(n1->get_input_plug(1),n2->get_output_plug(1));
////
////      link3->connect_plugs(n3->get_input_plug(0),n2->get_output_plug(0));
////      link4->connect_plugs(n3->get_input_plug(1),n2->get_output_plug(0));
////
////    //  n1->get_output_plug(0)->connect(n3->get_input_plug(0));
////    //  n1->get_output_plug(1)->connect(n3->get_input_plug(1));
////
////      //glm::vec2 pos;
////      pos.x = 300;
////      pos.y = 390;
////      n1->switch_to_node_properties()->set_anchor_pos(pos);
////
////      pos.x = 500;
////      pos.y = 390;
////      n1->switch_to_node_properties()->set_anchor_pos(pos);
////
////      pos.x = 330;
////      pos.y = 600;
////      n2->switch_to_node_properties()->set_anchor_pos(pos);
////
////      pos.x = 300;
////      pos.y = 140;
////      n3->switch_to_node_properties()->set_anchor_pos(pos);
////
////      pos.x = 400;
////      pos.y = 250;
////      n4->switch_to_node_properties()->set_anchor_pos(pos);
////
////      pos.x = 600;
////      pos.y = 700;
////      i1->switch_to_node_properties()->set_anchor_pos(pos);
////
////      pos.x = 600;
////      pos.y = 600;
////      o1->switch_to_node_properties()->set_anchor_pos(pos);
////
////    }
//}
//
//}
//
