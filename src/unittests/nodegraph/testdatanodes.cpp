//#include <unittests/nodegraph/testdatanodes.h>
//
//#include <base/objectmodel/entity.h>
//#include <base/memoryallocator/taggednew.h>
//#include <components/compute/compute.h>
//#include <components/compute/inputplug.h>
//#include <components/compute/outputplug.h>
//#include <components/interactions/nodegroup.h>
//#include <cstddef>
//#include <cassert>
//
//namespace open_node_graph {
//
//
//
//TestDataNodes::TestDataNodes():
//  _group_node(NULL),
//  _root_group(NULL),
//  _group(NULL),
//  _da(NULL),
//  _db(NULL),
//  _na(NULL),
//  _nb(NULL),
//  _nc(NULL),
//  _nd(NULL){
//
//  set_up();
//  test_nonrelay_nodes();
//  test_dot_nodes();
//  test_group_nodes();
//}
//
//TestDataNodes::~TestDataNodes() {
//  delete2_ff(_root_node->our_entity());
//}
//
//void TestDataNodes::set_up() {
//
//  // Create the ng helper and root entitites.
//  _ng_helper_entity = NonGuiFactory::build_app();
//  _factory = _ng_helper_entity->get<EntityFactory>();
//  _root_entity = *(_ng_helper_entity->get_children().begin());
//  Node* node = _root_entity->get<Node>();
//
//  _root_node = static_cast<GroupManager*>(node);
//  _root_group = _root_node->switch_to<NodeGroup>();
//  _root_link_group = _root_node->switch_to<LinkGroup>();
//
//  // Create the SubGroup Node.
//  _sub_group_entity = _factory->build_group_node(_root_entity, "group");
//  _group_node = _sub_group_entity->get<Node>();
//  _group = _group_node->switch_to<NodeGroup>();
//
//    // Test Nodes.
//	_na = _factory->build_mock_node(_root_entity, "na")->get<Node>();
//	_nb = _factory->build_mock_node(_root_entity, "nb")->get<Node>();
//	_nc = _factory->build_mock_node(_root_entity, "nc")->get<Node>();
//	_nd = _factory->build_mock_node(_root_entity, "nd")->get<Node>();
//
//    // Dot Nodes.
//	_da = _factory->build_dot_node(_root_entity, "da")->get<Node>();
//	_db = _factory->build_dot_node(_root_entity, "db")->get<Node>();
//
//    // Links.
//	_link1 = _factory->build_link(_root_entity)->get<Link>();
//	_link2 = _factory->build_link(_root_entity)->get<Link>();
//	_link3 = _factory->build_link(_root_entity)->get<Link>();
//	_link4 = _factory->build_link(_root_entity)->get<Link>();
//	_link5 = _factory->build_link(_root_entity)->get<Link>();
//	_link6 = _factory->build_link(_root_entity)->get<Link>();
//	_link7 = _factory->build_link(_root_entity)->get<Link>();
//	_link8 = _factory->build_link(_root_entity)->get<Link>();
//	_link9 = _factory->build_link(_root_entity)->get<Link>();
//
//    //_link1 = _root_link_group->create_link()->get_link();
//    //_link2 = _root_link_group->create_link()->get_link();
//    //_link3 = _root_link_group->create_link()->get_link();
//    //_link4 = _root_link_group->create_link()->get_link();
//    //_link5 = _root_link_group->create_link()->get_link();
//    //_link6 = _root_link_group->create_link()->get_link();
//    //_link7 = _root_link_group->create_link()->get_link();
//    //_link8 = _root_link_group->create_link()->get_link();
//    //_link9 = _root_link_group->create_link()->get_link();
//
//    // Get computer interfaces.
//    _na_computer = _na->switch_to<Compute>();
//    _nb_computer = _nb->switch_to<Compute>();
//    _nc_computer = _nc->switch_to<Compute>();
//    _nd_computer = _nd->switch_to<Compute>();
//    _da_computer = _da->switch_to<Compute>();
//    _db_computer = _db->switch_to<Compute>();
//}
//
//
//// Test configuration.
////         na
////       c    d
////      / \    \
////     a   b    a
////   nb     nc   nd
//
//void TestDataNodes::test_nonrelay_nodes() {
//    // All nodes should start out dirty.
//    assert(_na->is_dirty(NULL));
//    assert(_nb->is_dirty(NULL));
//    assert(_nc->is_dirty(NULL));
//    assert(_nd->is_dirty(NULL));
//
//    // ------------------------------------------------------
//    // Test individual cleans.
//    // ------------------------------------------------------
//
//    // Clean all the nodes.
//    _na_computer->clean_and_compute(NULL);
//    _nb_computer->clean_and_compute(NULL);
//    _nc_computer->clean_and_compute(NULL);
//    _nd_computer->clean_and_compute(NULL);
//    assert(_na->is_clean(NULL));
//    assert(_nb->is_clean(NULL));
//    assert(_nc->is_clean(NULL));
//    assert(_nd->is_clean(NULL));
//    assert(_na_computer->get_debug_result()==10);
//    assert(_nb_computer->get_debug_result()==10);
//    assert(_nc_computer->get_debug_result()==10);
//    assert(_nd_computer->get_debug_result()==10);
//
//    // ------------------------------------------------------
//    // Test propagation on connect.
//    // ------------------------------------------------------
//
//    // Handy references.
//    OutputPlug* na_c = _na->get_output_plug(0);
//    OutputPlug* na_d = _na->get_output_plug(1);
//    InputPlug* nb_a = _nb->get_input_plug(0);
//    InputPlug* nc_b = _nc->get_input_plug(1);
//    InputPlug* nd_a = _nd->get_input_plug(0);
//
//    // Start hooking everything up.
//    _link1->connect_plugs(nb_a, na_c);
//    assert(_na->is_clean(NULL));
//    assert(_nb->is_dirty(NULL));
//    assert(_nb_computer->get_debug_result()==0);
//    _link2->connect_plugs(nc_b, na_c);
//    assert(_na->is_clean(NULL));
//    assert(_nc->is_dirty(NULL));
//    assert(_nc_computer->get_debug_result()==0);
//    _link3->connect_plugs(nd_a, na_d);
//    assert(_na->is_clean(NULL));
//    assert(_nd->is_dirty(NULL));
//    assert(_nd_computer->get_debug_result()==0);
//
//    // ------------------------------------------------------
//    // Test simple cleans.
//    // ------------------------------------------------------
//
//    // Clean the downstream nodes one by one.
//    _nb_computer->clean_and_compute(NULL);
//    assert(_nb->is_clean(NULL));
//    assert(_nb_computer->get_debug_result()==10);
//    _nc_computer->clean_and_compute(NULL);
//    assert(_nc->is_clean(NULL));
//    assert(_nc_computer->get_debug_result()==10);
//    _nd_computer->clean_and_compute(NULL);
//    assert(_nd->is_clean(NULL));
//    assert(_nd_computer->get_debug_result()==10);
//
//    // ------------------------------------------------------
//    // Test propagation on node dirty.
//    // ------------------------------------------------------
//
//    // Check propagating dirtiness from na.
//    _na_computer->dirty(NULL);
//    assert(_na->is_dirty(NULL));
//    assert(_nb->is_dirty(NULL));
//    assert(_nc->is_dirty(NULL));
//    assert(_nd->is_dirty(NULL));
//    assert(_na_computer->get_debug_result()==0);
//    assert(_nb_computer->get_debug_result()==0);
//    assert(_nc_computer->get_debug_result()==0);
//    assert(_nd_computer->get_debug_result()==0);
//
//    // ------------------------------------------------------
//    // Test recursion on clean.
//    // ------------------------------------------------------
//
//    // Clean nb.
//    _nb_computer->clean_and_compute(NULL);
//    assert(_na->is_clean(NULL));
//    assert(_na_computer->get_debug_result()==10);
//    assert(_nb->is_clean(NULL));
//    assert(_nb_computer->get_debug_result()==10);
//    assert(_nc->is_dirty(NULL));
//    assert(_nc_computer->get_debug_result()==0);
//    assert(_nd->is_dirty(NULL));
//    assert(_nd_computer->get_debug_result()==0);
//
//    // Clean nc.
//    _na_computer->dirty(NULL);
//    _nc_computer->clean_and_compute(NULL);
//    assert(_na->is_clean(NULL));
//    assert(_na_computer->get_debug_result()==10);
//    assert(_nc->is_clean(NULL));
//    assert(_nc_computer->get_debug_result()==10);
//    assert(_nb->is_dirty(NULL));
//    assert(_nb_computer->get_debug_result()==0);
//    assert(_nd->is_dirty(NULL));
//    assert(_nd_computer->get_debug_result()==0);
//
//    // Clean nd.
//    _na_computer->dirty(NULL);
//    _nd_computer->clean_and_compute(NULL);
//    assert(_na->is_clean(NULL));
//    assert(_na_computer->get_debug_result()==10);
//    assert(_nd->is_clean(NULL));
//    assert(_nd_computer->get_debug_result()==10);
//    assert(_nb->is_dirty(NULL));
//    assert(_nb_computer->get_debug_result()==0);
//    assert(_nc->is_dirty(NULL));
//    assert(_nc_computer->get_debug_result()==0);
//
//    // ------------------------------------------------------
//    // Test propagation on disconnect.
//    // ------------------------------------------------------
//
//    // Disconnect nb and nc.
//    Link::disconnect_all_links(_na->get_output_plug(0));
//    assert(_na->is_clean(NULL));
//    assert(_na_computer->get_debug_result()==10);
//    assert(_nb->is_dirty(NULL));
//    assert(_nb_computer->get_debug_result()==0);
//    assert(_nc->is_dirty(NULL));
//    assert(_nc_computer->get_debug_result()==0);
//
//    // Disconnect nd.
//    Link::disconnect_link(_nd->get_input_plug(0));
//    assert(_na->is_clean(NULL));
//    assert(_na_computer->get_debug_result()==10);
//    assert(_nd->is_dirty(NULL));
//    assert(_nd_computer->get_debug_result()==0);
//}
//
//// Test configuration.
////         na
////       c    d
////       |     \
////       da     db
////      / \      \
////     a   b      a
////   nb     nc     nd
//
//void TestDataNodes::test_dot_nodes() {
//  // From the previous test all node except na will be dirty.
//  // They should all be disconnected also.
//  _na_computer->clean_and_compute(NULL);
//  _nb_computer->clean_and_compute(NULL);
//  _nc_computer->clean_and_compute(NULL);
//  _nd_computer->clean_and_compute(NULL);
//
//  // The dot nodes should start out dirty.
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_dirty(NULL));
//  _da_computer->clean_and_compute(NULL);
//  _db_computer->clean_and_compute(NULL);
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//
//  // Handy references.
//  OutputPlug* na_c = _na->get_output_plug(0);
//  OutputPlug* na_d = _na->get_output_plug(1);
//  InputPlug* nb_a = _nb->get_input_plug(0);
//  InputPlug* nc_b = _nc->get_input_plug(1);
//  InputPlug* nd_a = _nd->get_input_plug(0);
//
//  // --------------------------------------------------------------------
//  // Here we start hooking up the dot nodes.
//  // --------------------------------------------------------------------
//
//  // Connect up da to na_c.
//  _link1->connect_plugs(_da->get_input_plug(0),na_c);
//  assert(_da->is_dirty(NULL));
//  assert(_na->is_clean(NULL));
//
//  // Connect up db to na_d.
//  _link2->connect_plugs(_db->get_input_plug(0),na_d);
//  assert(_db->is_dirty(NULL));
//  assert(_na->is_clean(NULL));
//
//  // Clean da and db.
//  _da_computer->clean_and_compute(NULL);
//  assert(_da->is_clean(NULL));
//  _db_computer->clean_and_compute(NULL);
//  assert(_db->is_clean(NULL));
//
//  // Make sure na is still clean.
//  assert(_na->is_clean(NULL));
//
//  // --------------------------------------------------------------------
//  // At this point the graph from na down to da and db has been built.
//  // --------------------------------------------------------------------
//
//  // Dirty na, and check propagation.
//  _na_computer->dirty(NULL);
//  assert(_na->is_dirty(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_dirty(NULL));
//
//  // Clean da.
//  _da_computer->clean_and_compute(NULL);
//  assert(_da->is_clean(NULL));
//  assert(_na->is_clean(NULL));
//  assert(_db->is_dirty(NULL));
//
//  // Clean db.
//  _db_computer->clean_and_compute(NULL);
//  assert(_db->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_na->is_clean(NULL));
//
//  // --------------------------------------------------------------------
//  // Here we start connecting up the terminal nodes.
//  // --------------------------------------------------------------------
//
//  // Now lets hook up nb.
//  assert(!_nb->is_dirty(NULL));
//  _link3->connect_plugs(nb_a,_da->get_output_plug(0));
//  assert(_nb->is_dirty(NULL));
//  assert(!_da->is_dirty(NULL));
//
//  // Now lets hook up nc.
//  assert(_nc->is_clean(NULL));
//  _link4->connect_plugs(nc_b,_da->get_output_plug(0));
//  assert(_nc->is_dirty(NULL));
//  assert(_da->is_clean(NULL));
//
//  // Now lets hook up nd.
//  assert(_nd->is_clean(NULL));
//  _link5->connect_plugs(nd_a,_db->get_output_plug(0));
//  assert(_nd->is_dirty(NULL));
//  assert(_da->is_clean(NULL));
//
//  // --------------------------------------------------------------------
//  // At this point all the terminal nodes are hooked up.
//  // --------------------------------------------------------------------
//
//  // Now clean all the terminal nodes.
//  _nb_computer->clean_and_compute(NULL);
//  _nc_computer->clean_and_compute(NULL);
//  _nd_computer->clean_and_compute(NULL);
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Everything should be clean now.
//  assert(_na->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Dirty everything from a.
//  _na_computer->dirty(NULL);
//  assert(_na->is_dirty(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_dirty(NULL));
//
//  // Clean nb.
//  _nb_computer->clean_and_compute(NULL);
//  assert(_na->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_dirty(NULL));
//
//  // Dirty everything from a.
//  _na_computer->dirty(NULL);
//  assert(_na->is_dirty(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_dirty(NULL));
//
//  // Clean nc.
//  _nc_computer->clean_and_compute(NULL);
//  assert(_na->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_dirty(NULL));
//
//  // Dirty everything from a.
//  _na_computer->dirty(NULL);
//  assert(_na->is_dirty(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_dirty(NULL));
//
//  // Clean nd.
//  _nd_computer->clean_and_compute(NULL);
//  assert(_na->is_clean(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_clean(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Clean nc and nd to make everything clean now.
//
//  _nc_computer->clean_and_compute(NULL);
//  assert(_na->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  _nb_computer->clean_and_compute(NULL);
//  assert(_na->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // --------------------------------------------------------------------
//  // For the reset of this test we break down this graph.
//  // --------------------------------------------------------------------
//
//  // --------------------------------------------------------------------
//  // Create two subgraphs. 1) da->nb,nc. 2) db->nd.
//  // --------------------------------------------------------------------
//
//  // Disconnect na_c->da.
//  _link1->disconnect_both_ends();//(_da->get_input_plug(0),na_c);
//  assert(_na->is_clean(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_db->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Disconnect na_d->db.
//  _link2->disconnect_both_ends();//->disconnect(_db->get_input_plug(0),na_d);
//  assert(_na->is_clean(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(_nd->is_dirty(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//
//  // --------------------------------------------------------------------
//  // Work on subgraph (1).
//  // --------------------------------------------------------------------
//
//  // Clean nb.
//  _nb_computer->clean_and_compute(NULL);
//  assert(_da->is_clean(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_dirty(NULL));
//
//  // Make da dirty.
//  _da_computer->dirty(NULL);
//  assert(_da->is_dirty(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//
//  // Clean nc.
//  _nc_computer->clean_and_compute(NULL);
//  assert(_da->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nb->is_dirty(NULL));
//
//  // Clean nb.
//  _nb_computer->clean_and_compute(NULL);
//  assert(_da->is_clean(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//
//  // Disconnect nb.
//  _link3->disconnect_both_ends(); //nb_a->disconnect();
//  assert(_da->is_clean(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_clean(NULL));
//
//  // Disconnect nc.
//  _link4->disconnect_both_ends(); //nc_b->disconnect();
//  assert(_da->is_clean(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//
//  // --------------------------------------------------------------------
//  // Work on subgraph (2).
//  // --------------------------------------------------------------------
//
//  // Clean nd.
//  _nd_computer->clean_and_compute(NULL);
//  assert(_db->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Disconnect nd.
//  _link5->disconnect_both_ends(); //nd_a->disconnect();
//  assert(_db->is_clean(NULL));
//  assert(_nd->is_dirty(NULL));
//
//  // --------------------------------------------------------------------
//  // At this point all the nodes are split apart.
//  // --------------------------------------------------------------------
//
//  // Return the clean nodes back to being dirty.
//  _da_computer->dirty(NULL);
//  _db_computer->dirty(NULL);
//  _na_computer->dirty(NULL);
//
//  // Verify all nodes are dirty.
//  assert(_na->is_dirty(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_dirty(NULL));
//}
//
//// Test configuration.
////          na
////         c   d
////         |   |
////        ga   gb
////     -------------
////    |    /   \    |
////    |  da     db  |  group
////    |  |      |   |
////     -------------
////       gc     gd
////      / \      \
////     a   b      a
////   nb     nc     nd
////
//// Note: we'll be mostly testing the group and it's relay node functionality.
////       we'll try to avoid duplicating tests which have already been covered.
//
//void TestDataNodes::test_group_nodes() {
//  // All the nodes should be disconnected from each other at this point and dirty.
//  EntityFactory* factory = _sub_group_entity->get_app_entity()->get<EntityFactory>();
//  // Add the input plugs to the group.
//  InputPlug* ga = factory->build_input_plug(_sub_group_entity, "a", kQVariant)->get<InputPlug>();
//  InputPlug* gb = factory->build_input_plug(_sub_group_entity, "b", kQVariant)->get<InputPlug>();
//  OutputPlug* gc = factory->build_output_plug(_sub_group_entity, "c", kQVariant)->get<OutputPlug>();
//  OutputPlug* gd = factory->build_output_plug(_sub_group_entity, "d", kQVariant)->get<OutputPlug>();
//
//  Node* input_node_a = factory->build_input_node(_sub_group_entity, "a")->get<Node>();
//  Node* input_node_b = factory->build_input_node(_sub_group_entity, "b")->get<Node>();
//  Node* output_node_c = factory->build_output_node(_sub_group_entity, "c")->get<Node>();
//  Node* output_node_d = factory->build_output_node(_sub_group_entity, "d")->get<Node>();
//
//  //Node* input_node_a = _group->build_node(kInputNodeEntity, "a")->get<Node>();
//  //Node* input_node_b = _group->build_node(kInputNodeEntity, "b")->get<Node>();
//  //Node* output_node_c = _group->build_node(kOutputNodeEntity,"c")->get<Node>();
//  //Node* output_node_d = _group->build_node(kOutputNodeEntity,"d")->get<Node>();
//
//  // Handy references.
//  OutputPlug* na_c = _na->get_output_plug(0);
//  OutputPlug* na_d = _na->get_output_plug(1);
//  InputPlug* nb_a = _nb->get_input_plug(0);
//  InputPlug* nc_b = _nc->get_input_plug(1);
//  InputPlug* nd_a = _nd->get_input_plug(0);
//  InputPlug* g_a = _group_node->get_input_plug(0);
//  InputPlug* g_b = _group_node->get_input_plug(1);
//  OutputPlug* g_c = _group_node->get_output_plug(0);
//  OutputPlug* g_d = _group_node->get_output_plug(1);
//  assert(gc != g_d);
//  OutputPlug* input_node_output_plug_a = input_node_a->get_output_plug(0);
//  OutputPlug* input_node_output_plug_b = input_node_b->get_output_plug(0);
//  InputPlug* output_node_input_plug_c = output_node_c->get_input_plug(0);
//  InputPlug* output_node_input_plug_d = output_node_d->get_input_plug(0);
//  InputPlug* da_in = _da->get_input_plug(0);
//  OutputPlug* da_out = _da->get_output_plug(0);
//  InputPlug* db_in = _db->get_input_plug(0);
//  OutputPlug* db_out = _db->get_output_plug(0);
//
//  Compute* up_dot_a_computer = input_node_a->switch_to<Compute>();
//  Compute* up_dot_b_computer = input_node_b->switch_to<Compute>();
//  Compute* down_dot_c_computer = output_node_c->switch_to<Compute>();
//  Compute* down_dot_d_computer = output_node_d->switch_to<Compute>();
//
//  // Clean the dot nodes.
//  _da_computer->clean_and_compute(NULL);
//  _db_computer->clean_and_compute(NULL);
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//
//  // Remove the dot nodes from the root group.
//  assert(_da->get_parent_group()==_root_group);
//  assert(_db->get_parent_group()==_root_group);
//  _root_group->remove_node(_da->our_entity());
//  _root_group->remove_node(_db->our_entity());
//  assert(_da->get_parent_group()==NULL);
//  assert(_db->get_parent_group()==NULL);
//
//  // Add the dot nodes into the group.
//  assert(true==_group->add_node(_da->our_entity()));
//  assert(true==_group->add_node(_db->our_entity()));
//  assert(_group->is_member(_da->our_entity()));
//  assert(_group->is_member(_db->our_entity()));
//  assert(_da->get_parent_group()==_group);
//  assert(_db->get_parent_group()==_group);
//
//  // Adding nodes to a group doesn't change their cleanliness.
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//
//  // Make sure the input nodes are clean.
//  assert(input_node_a->is_dirty(NULL));
//  assert(input_node_b->is_dirty(NULL));
//  up_dot_a_computer->clean_and_compute(NULL);
//  assert(input_node_a->is_clean(NULL));
//  up_dot_b_computer->clean_and_compute(NULL);
//  assert(input_node_b->is_clean(NULL));
//
//  // Hook up the input nodes to the dot nodes.
//  _link3->connect_plugs(da_in,input_node_output_plug_a);
//  assert(_da->is_dirty(NULL));
//  assert(input_node_a->is_clean(NULL));
//  _link4->connect_plugs(db_in,input_node_output_plug_b);
//  assert(_db->is_dirty(NULL));
//  assert(input_node_b->is_clean(NULL));
//
//  // Clean the dot nodes.
//  _da_computer->clean_and_compute(NULL);
//  assert(_da->is_clean(NULL));
//  _db_computer->clean_and_compute(NULL);
//  assert(_db->is_clean(NULL));
//
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//
//  // Dirty the input nodes to test dirty propagation.
//  up_dot_a_computer->dirty(NULL);
//  assert(input_node_a->is_dirty(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  up_dot_b_computer->dirty(NULL);
//  assert(input_node_b->is_dirty(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(input_node_a->is_dirty(NULL));
//  assert(_da->is_dirty(NULL));
//
//  // Clean the dot nodes.
//  _da_computer->clean_and_compute(NULL);
//  assert(_da->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(input_node_b->is_dirty(NULL));
//  _db_computer->clean_and_compute(NULL);
//  assert(_db->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//
//  // Clean the output nodes.
//  assert(output_node_c->is_dirty(NULL));
//  assert(output_node_d->is_dirty(NULL));
//  down_dot_c_computer->clean_and_compute(NULL);
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_dirty(NULL));
//  down_dot_d_computer->clean_and_compute(NULL);
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//
//  // Hook up the output node c.
//  _link5->connect_plugs(output_node_input_plug_c,da_out);
//  assert(_da->is_clean(NULL));
//  assert(output_node_c->is_dirty(NULL));
//  down_dot_c_computer->clean_and_compute(NULL);
//
//  // Hook up the output node d.
//  _link6->connect_plugs(output_node_input_plug_d,db_out);
//  assert(_db->is_clean(NULL));
//  assert(output_node_d->is_dirty(NULL));
//  down_dot_d_computer->clean_and_compute(NULL);
//
//  // Make everything dirty from upstream dot a.
//  up_dot_a_computer->dirty(NULL);
//  assert(input_node_a->is_dirty(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_dirty(NULL));
//  assert(output_node_d->is_clean(NULL));
//
//  // Make everything dirty from upstream dot b.
//  up_dot_b_computer->dirty(NULL);
//  assert(input_node_a->is_dirty(NULL));
//  assert(input_node_b->is_dirty(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(output_node_c->is_dirty(NULL));
//  assert(output_node_d->is_dirty(NULL));
//
//  // -------------------------------------------------------------------
//  // At this point the internals of the group are fully hooked up.
//  // -------------------------------------------------------------------
//
//  // -------------------------------------------------------------------
//  // Hook up nb, nc, nd.
//  // -------------------------------------------------------------------
//
//  // Clean the internals of the group.
//  down_dot_c_computer->clean_and_compute(NULL);
//  down_dot_d_computer->clean_and_compute(NULL);
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//
//  // Hook up nb.
//  _link7->connect_plugs(nb_a,g_c);
//  assert(_nb->is_dirty(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//
//  // Hook up nc.
//  _link8->connect_plugs(nc_b,g_c);
//  assert(_nc->is_dirty(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//
//  // Hook up nd.
//  _link9->connect_plugs(nd_a,g_d);
//  assert(_nd->is_dirty(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//
//  // Make everything clean.
//  _nb_computer->clean_and_compute(NULL);
//  assert(_nb->is_clean(NULL));
//  _nc_computer->clean_and_compute(NULL);
//  assert(_nc->is_clean(NULL));
//  _nd_computer->clean_and_compute(NULL);
//  assert(_nd->is_clean(NULL));
//
//  // -------------------------------------------------------------------
//  // Hook up na.
//  // -------------------------------------------------------------------
//
//  // Hook up na_c to the group.
//  _link1->connect_plugs(g_a,na_c);
//  assert(_na->is_dirty(NULL));
//  assert(input_node_a->is_dirty(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_dirty(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Clean everything.
//  _nb_computer->clean_and_compute(NULL);
//  _nc_computer->clean_and_compute(NULL);
//  _nd_computer->clean_and_compute(NULL);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Hook up na_d to the group.
//  _link2->connect_plugs(g_b,na_d);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_dirty(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_dirty(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_dirty(NULL));
//
//  // Clean everything.
//  _nb_computer->clean_and_compute(NULL);
//  _nc_computer->clean_and_compute(NULL);
//  _nd_computer->clean_and_compute(NULL);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // -------------------------------------------------------------------
//  // At this point the full graph is setup.
//  // -------------------------------------------------------------------
//
////  // Sneek in some testing of the global methods to get the nondot input and output plugs.
////  {
////    assert(na_c == get_real_output_plug(g_c));
////    assert(na_d == get_real_output_plug(g_d));
////    assert(na_c == get_real_output_plug(da_out));
////    assert(na_d == get_real_output_plug(db_out));
////    assert(na_c == get_real_output_plug(input_node_output_plug_a));
////    assert(na_d == get_real_output_plug(input_node_output_plug_b));
////    std::vector<InputPlug*> input_plugs = get_real_input_plugs(g_a);
////    std::vector<InputPlug*> input_plugs2 = get_real_input_plugs(g_b);
////    std::vector<InputPlug*> input_plugs3 = get_real_input_plugs(da_in);
////    std::vector<InputPlug*> input_plugs4 = get_real_input_plugs(db_in);
////    std::vector<InputPlug*> input_plugs5 = get_real_input_plugs(output_node_input_plug_c);
////    std::vector<InputPlug*> input_plugs6 = get_real_input_plugs(output_node_input_plug_d);
////
////    // check input_plugs.
////    assert(input_plugs.size()==2);
////    assert(input_plugs[0]==nb_a);
////    assert(input_plugs[1]==nc_b);
////    // check input_plugs2.
////    assert(input_plugs2.size()==1);
////    assert(input_plugs2[0]==nd_a);
////    // check input_plug3.
////    assert(input_plugs3.size()==2);
////    assert(input_plugs3[0]==nb_a);
////    assert(input_plugs3[1]==nc_b);
////    // check input_plugs4.
////    assert(input_plugs4.size()==1);
////    assert(input_plugs4[0]==nd_a);
////    // check input_plugs5.
////    assert(input_plugs5.size()==2);
////    assert(input_plugs5[0]==nb_a);
////    assert(input_plugs5[1]==nc_b);
////    // check input_plugs6.
////    assert(input_plugs6.size()==1);
////    assert(input_plugs6[0]==nd_a);
////
////  }
//  // Finish the sneek testing of the global methods.
//
//
//  // Check dirty propagation from na.
//  _na_computer->dirty(NULL);
//  assert(_na->is_dirty(NULL));
//  assert(input_node_a->is_dirty(NULL));
//  assert(input_node_b->is_dirty(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(output_node_c->is_dirty(NULL));
//  assert(output_node_d->is_dirty(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_dirty(NULL));
//
//  // Clean everything.
//  _nb_computer->clean_and_compute(NULL);
//  _nc_computer->clean_and_compute(NULL);
//  _nd_computer->clean_and_compute(NULL);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Check dirty propagation from da, which is inside group.
//  _da_computer->dirty(NULL);
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_dirty(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_clean(NULL)); // With optimized relay nodes this could be optimized so it stays clean.
//
//  // Clean everything.
//  _nb_computer->clean_and_compute(NULL);
//  _nc_computer->clean_and_compute(NULL);
//  _nd_computer->clean_and_compute(NULL);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Check dirty propagation from db, which is inside group.
//  _db_computer->dirty(NULL);
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_dirty(NULL));
//  assert(_nb->is_clean(NULL)); // With optimized relay nodes this could be optimized so it stays clean.
//  assert(_nc->is_clean(NULL)); // With optimized relay nodes this could be optimized so it stays clean.
//  assert(_nd->is_dirty(NULL));
//
//  // Clean everything.
//  _nb_computer->clean_and_compute(NULL);
//  _nc_computer->clean_and_compute(NULL);
//  _nd_computer->clean_and_compute(NULL);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Disconnect na_c.
//  Link::disconnect_all_links(na_c);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_dirty(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_dirty(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Disconnect na_d.
//  Link::disconnect_all_links(na_d);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_dirty(NULL));
//  assert(input_node_b->is_dirty(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(output_node_c->is_dirty(NULL));
//  assert(output_node_d->is_dirty(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_dirty(NULL));
//
//  // Clean everything.
//  _nb_computer->clean_and_compute(NULL);
//  _nc_computer->clean_and_compute(NULL);
//  _nd_computer->clean_and_compute(NULL);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Disconnect da.
//  Link::disconnect_link(da_in);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_dirty(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Disconnect db.
//  Link::disconnect_link(db_in);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_dirty(NULL));
//  assert(_db->is_dirty(NULL));
//  assert(output_node_c->is_dirty(NULL));
//  assert(output_node_d->is_dirty(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_dirty(NULL));
//
//  // Clean everything.
//  _nb_computer->clean_and_compute(NULL);
//  _nc_computer->clean_and_compute(NULL);
//  _nd_computer->clean_and_compute(NULL);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Disconnect down_c.
//  Link::disconnect_link(output_node_input_plug_c);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_dirty(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Disconnect down_dot_b.
//  Link::disconnect_link(output_node_input_plug_d);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_dirty(NULL));
//  assert(output_node_d->is_dirty(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_dirty(NULL));
//
//  // Clean everything.
//  _nb_computer->clean_and_compute(NULL);
//  _nc_computer->clean_and_compute(NULL);
//  _nd_computer->clean_and_compute(NULL);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_clean(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Disconnect nb.
//  Link::disconnect_link(nb_a);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_clean(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Disconnect nc.
//  Link::disconnect_link(nc_b);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_clean(NULL));
//
//  // Disconnect nd.
//  Link::disconnect_link(nd_a);
//  assert(_na->is_clean(NULL));
//  assert(input_node_a->is_clean(NULL));
//  assert(input_node_b->is_clean(NULL));
//  assert(_da->is_clean(NULL));
//  assert(_db->is_clean(NULL));
//  assert(output_node_c->is_clean(NULL));
//  assert(output_node_d->is_clean(NULL));
//  assert(_nb->is_dirty(NULL));
//  assert(_nc->is_dirty(NULL));
//  assert(_nd->is_dirty(NULL));
//
//  // ------------------------------------------------------------------------
//  // At this point everything is disconnected.
//  // ------------------------------------------------------------------------
//
//  // Remove da from the group.
//  assert(_da->get_parent_group() == _group);
//  _group->remove_node(_da->our_entity());
//  assert(_da->get_parent_group() == NULL);
//  _root_group->add_node(_da->our_entity());
//  assert(_da->get_parent_group() == _root_group);
//
//  // Remove db from the group.
//  assert(_db->get_parent_group() == _group);
//  _group->remove_node(_db->our_entity());
//  assert(_db->get_parent_group() == NULL);
//  _root_group->add_node(_db->our_entity());
//  assert(_db->get_parent_group() == _root_group);
//
//}
//
//}
