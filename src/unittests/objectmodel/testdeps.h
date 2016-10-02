#pragma once
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>

namespace ngs {

class InputCompute;
class OutputCompute;
class Compute;
class BaseFactory;

/*
 * This class tests the dependencies in the following configuration.
 *
 *  DotNode Entity
 *  - dotnodecompute
 *  - inputs/in child
 *    - inputplug
 *    - inputparamcompute
 *    - outputplug
 *  - outputs/out child
 *    - outputparamcompute
 *    - outputplug
 *
 *    MockNode Entity
 *    - mocknodecompute
 *     *  - inputs/a, inputs/b children
 *    - inputplug
 *    - inputparamcompute
 *    - outputplug
 *  - outputs/c, outputs/d children
 *    - outputparamcompute
 *    - outputplug
 *
 *
 *
 *       dotA/inputs/in:inputcompute       dotB/inputs/in:inputcompute
 *                   |                                  |
 *             dotA:compute                       dotB:compute
 *                   |                                  |
 *       dotA/outputs/out:outputcompute    dotB/outputs/out:outputcompute
 *                   |                                  |
 *    -------------- | ----------- mock node ---------- | ------------------
 *    |              |                                  |                  |
 *    |  mock/inputs/a:inputcompute        mock/inputs/b:inputcompute      |
 *    |                          \          /                              |
 *    |                           mock:compute                             |
 *    |                          /          \                              |
 *    |  mock/outputs/c:outputcompute      mock/outputs/d:outputcompute    |
 *    |              |                                  |                  |
 *    -------------- | -------------------------------- | ------------------
 *                   |                                  |
 *       dotC/inputs/in:inputcompute       dotD/inputs/in:inputcompute
 *                   |                                  |
 *             dotC:compute                       dotD:compute
 *                   |                                  |
 *       dotC/outputs/out:outputcompute    dotD/outputs/out:outputcompute
 *
 */

class TestDeps: public Component {
 public:

  COMPONENT_ID(InvalidComponent, InvalidComponent)

  TestDeps(Entity* entity);
  ~TestDeps();

  void test();

 private:

  void set_up();
  void test_connecting_plugs();
  void test_cycle_detection();
  void test_disconnecting_plugs();
  void test_collapsing_to_group();
  void test_group_compute();
  void test_saving();
  void test_loading();
  void test_dep_low_level();

  void verify(std::unordered_set<Component*> cs, bool dirty);
  void verify_all(bool dirty);

  void clean_all_components();
  void dirty_all_components();



  // Our fixed deps.
  Dep<BaseFactory> _factory;

  // Nodes.
  Entity* _mock;
  Entity* _dot_a;
  Entity* _dot_b;
  Entity* _dot_c;
  Entity* _dot_d;

  // Node Computes.
  Dep<Compute> _mock_compute;
  Dep<Compute> _dot_a_compute;
  Dep<Compute> _dot_b_compute;
  Dep<Compute> _dot_c_compute;
  Dep<Compute> _dot_d_compute;

  // Plug Computes.
  Dep<InputCompute> _mock_a_compute;
  Dep<InputCompute> _mock_b_compute;
  Dep<OutputCompute> _mock_c_compute;
  Dep<OutputCompute> _mock_d_compute;

  Dep<InputCompute> _dot_a_in_compute;
  Dep<OutputCompute> _dot_a_out_compute;

  Dep<InputCompute> _dot_b_in_compute;
  Dep<OutputCompute> _dot_b_out_compute;

  Dep<InputCompute> _dot_c_in_compute;
  Dep<OutputCompute> _dot_c_out_compute;

  Dep<InputCompute> _dot_d_in_compute;
  Dep<OutputCompute> _dot_d_out_compute;
};
}
