//#pragma once
//
//
//namespace open_node_graph {
//
//class Node;
//class GroupManager;
//class MockNode;
//class DotNode;
//class Link;
//class Compute;
//
//// This class tests the dirtiness and clean propagations between nodes.
//// The node types tested are MockNode, DotNode, and GroupNode.
//// Connecting and disconnecting plugs should automatically propagate dirtiness to appropriate nodes.
//// Cleaning a node should recursively clean all upstream dependencies first.
//
//class TestDataNodes {
// public:
//  TestDataNodes();
//  ~TestDataNodes();
//
//  void set_up();
//  void test_nonrelay_nodes();
//  void test_dot_nodes();
//  void test_group_nodes();
//
// private:
//  //Entities.
//  Entity* _ng_helper_entity;
//  Entity* _root_entity;
//  Entity* _sub_group_entity;
//
//  EntityFactory* _factory;
//
//  Node *_root_node,*_group_node;
//  NodeGroup* _root_group, *_group;
//  LinkGroup* _root_link_group;
//  Node *_na, *_nb, *_nc, *_nd;
//  Node *_da, *_db;
//  Link *_link1, *_link2, *_link3, *_link4, *_link5, *_link6, *_link7, *_link8, *_link9;
//
//  Compute *_na_computer, *_nb_computer, *_nc_computer, *_nd_computer;
//  Compute *_da_computer, *_db_computer;
//};
//
//}
