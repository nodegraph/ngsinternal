//#include <unittests/nodegraph/testplugs.h>
//
//#include <base/objectmodel/entity.h>
//#include <components/factories/nonguifactory.h>
//
//#include <base/memoryallocator/taggednew.h>
//#include <components/compute/inputplug.h>
//#include <components/compute/outputplug.h>
//#include <components/interactions/nodegroup.h>
//#include <cassert>
//
//namespace open_node_graph {
//
//TestPlugs::TestPlugs() {
//  set_up();
//  test_single_connection();
//  test_multi_connections();
//}
//
//TestPlugs::~TestPlugs() {
//  delete2_ff(_root_node->our_entity());
//}
//
//void TestPlugs::set_up() {
//  _ng_helper_entity = NonGuiFactory::build_app();
//  _factory = _ng_helper_entity->get<EntityFactory>();
//
//  _root_entity = *(_ng_helper_entity->get_children().begin());
//  Node* node = _root_entity->get<Node>();
//
//  _root_node = static_cast<GroupManager*>(node);
//  NodeGroup* node_group = _root_node->switch_to<NodeGroup>();
//
//  _na = node_group->switch_to<EntityFactory>()->build_mock_node(_root_entity, "na")->get<Node>();
//  _nb = node_group->switch_to<EntityFactory>()->build_mock_node(_root_entity, "nb")->get<Node>();
//  _nc = node_group->switch_to<EntityFactory>()->build_mock_node(_root_entity, "nc")->get<Node>();
//}
//
//void TestPlugs::test_single_connection() {
//
//    // Handy references.
//    NodeGroup* group = _root_node->switch_to<NodeGroup>();
//    LinkGroup* link_group = _root_node->switch_to<LinkGroup>();
//
//    OutputPlug* output_plug = _na->get_output_plug(0);
//    InputPlug* input_plug = _nb->get_input_plug(0);
//
//    OutputPlug* output_plug2 = _na->get_output_plug(1);
//    InputPlug* input_plug2 = _nb->get_input_plug(1);
//
//    Link* link = _factory->build_link(_root_entity)->get<Link>();
//    //Link* link = link_group->create_link()->get_link();
//
//    // Connect the link to the input plug.
//    link->connect_input_plug(input_plug);
//    assert(link->get_input_plug()==input_plug);
//    assert(input_plug->get_link()==link);
//
//    // Connect the link to the output plug.
//    link->connect_output_plug(output_plug);
//    assert(link->get_output_plug()==output_plug);
//    assert(output_plug->has_link(link));
//
//    // Disconnect the link from the input plug.
//    link->disconnect_input();
//    assert(input_plug->get_link()!=link);
//    assert(link->get_input_plug()==NULL);
//    assert(output_plug->has_link(link));
//    assert(link->get_output_plug()==output_plug);
//
//    // Disconnect the link from the output plug.
//    link->disconnect_output();
//    assert(input_plug->get_link()!=link);
//    assert(link->get_input_plug()==NULL);
//    assert(!output_plug->has_link(link));
//    assert(link->get_output_plug()==NULL);
//
//    // Make the connection to the output plug.
//    link->connect_output_plug(output_plug);
//    assert(link->get_output_plug()==output_plug);
//    assert(output_plug->has_link(link));
//
//    // Make the connection to the input plug.
//    link->connect_input_plug(input_plug);
//    assert(link->get_input_plug()==input_plug);
//    assert(input_plug->get_link()==link);
//
//    // Try making input connection to a novel input plug.
//    assert(true == link->connect_input_plug(input_plug2));
//    assert(link->get_input_plug()==input_plug2);
//    assert(input_plug2->get_link()==link);
//    assert(input_plug->get_link()==NULL);
//
//    // Try making output connection to a novel output plug.
//    assert(true == link->connect_output_plug(output_plug2));
//    assert(link->get_output_plug()==output_plug2);
//    assert(output_plug2->has_link(link));
//    assert(false == output_plug->has_link(link));
//
//    // Disconnect the link from the output plug.
//    link->disconnect_output();
//    assert(input_plug2->get_link()==link);
//    assert(link->get_input_plug()==input_plug2);
//    assert(!output_plug2->has_link(link));
//    assert(link->get_output_plug()==NULL);
//
//    // Disconnect the link from the input plug.
//    link->disconnect_input();
//    assert(input_plug2->get_link()!=link);
//    assert(link->get_input_plug()==NULL);
//    assert(!output_plug2->has_link(link));
//    assert(link->get_output_plug()==NULL);
//}
//
//void TestPlugs::test_multi_connections() {
//    // Handy references.
//    NodeGroup* group = _root_node->switch_to<NodeGroup>();
//    LinkGroup* link_group = _root_node->switch_to<LinkGroup>();
//
//    OutputPlug* output_plug = _na->get_output_plug(0);
//    InputPlug* input_plug1 = _nb->get_input_plug(0);
//    InputPlug* input_plug2 = _nc->get_input_plug(0);
//    InputPlug* input_plug3 = _nc->get_input_plug(1);
//
//	Link* link1 = _factory->build_link(_root_entity)->get<Link>();
//	Link* link2 = _factory->build_link(_root_entity)->get<Link>();
//	Link* link3 = _factory->build_link(_root_entity)->get<Link>();
//
//    //Link* link1 = link_group->create_link()->get_link();
//    //Link* link2 = link_group->create_link()->get_link();
//    //Link* link3 = link_group->create_link()->get_link();
//
//    // Connect up the output ends of the links.
//    link1->connect_output_plug(output_plug);
//    assert(output_plug->get_num_links()==1);
//    link2->connect_output_plug(output_plug);
//    assert(output_plug->get_num_links()==2);
//    link3->connect_output_plug(output_plug);
//    assert(output_plug->get_num_links()==3);
//
//    // Connect up the input ends of the links.
//    link1->connect_input_plug(input_plug1);
//    link2->connect_input_plug(input_plug2);
//    link3->connect_input_plug(input_plug3);
//
//    // Check the output plug.
//    std::vector<InputPlug*> input_plugs;
//    output_plug->get_input_plugs(input_plugs);
//    assert(input_plugs[0]==input_plug1);
//    assert(input_plugs[1]==input_plug2);
//    assert(input_plugs[2]==input_plug3);
//
//    // Check the input plugs.
//    assert(input_plug1->is_connected(output_plug));
//    assert(input_plug2->is_connected(output_plug));
//    assert(input_plug3->is_connected(output_plug));
//
//    // Break the links from the output.
//    Link::disconnect_all_links(output_plug);
//    assert(!output_plug->is_connected(input_plug1));
//    assert(!output_plug->is_connected(input_plug2));
//    assert(!output_plug->is_connected(input_plug3));
//    assert(!input_plug1->is_connected(output_plug));
//    assert(!input_plug2->is_connected(output_plug));
//    assert(!input_plug3->is_connected(output_plug));
//
//    // Connect the input ends of the links.
//    link1->connect_output_plug(output_plug);
//    assert(output_plug->get_num_links()==1);
//    link2->connect_output_plug(output_plug);
//    assert(output_plug->get_num_links()==2);
//    link3->connect_output_plug(output_plug);
//    assert(output_plug->get_num_links()==3);
//
//    // Connect the output ends of the links.
//    link1->connect_input_plug(input_plug1);
//    link2->connect_input_plug(input_plug2);
//    link3->connect_input_plug(input_plug3);
//
//    // Check the links.
//    assert(output_plug->is_connected(input_plug1));
//    assert(output_plug->is_connected(input_plug2));
//    assert(output_plug->is_connected(input_plug3));
//    assert(input_plug1->is_connected(output_plug));
//    assert(input_plug2->is_connected(output_plug));
//    assert(input_plug3->is_connected(output_plug));
//    assert(link1->both_ends_connected());
//    assert(link2->both_ends_connected());
//    assert(link3->both_ends_connected());
//
//    // Break the links from the input.
//    assert(output_plug->get_num_links()==3);
//    link1->disconnect_both_ends();
//    assert(output_plug->get_num_links()==2);
//    link2->disconnect_both_ends();
//    assert(output_plug->get_num_links()==1);
//    link3->disconnect_both_ends();
//    assert(output_plug->get_num_links()==0);
//
//    // Check the links.
//    assert(!output_plug->is_connected(input_plug1));
//    assert(!output_plug->is_connected(input_plug2));
//    assert(!output_plug->is_connected(input_plug3));
//    assert(!input_plug1->is_connected(output_plug));
//    assert(!input_plug2->is_connected(output_plug));
//    assert(!input_plug3->is_connected(output_plug));
//    assert(link1->both_ends_disconnected());
//    assert(link2->both_ends_disconnected());
//    assert(link3->both_ends_disconnected());
//}
//
//}
