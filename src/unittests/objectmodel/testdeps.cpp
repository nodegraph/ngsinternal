#include <unittests/objectmodel/testdeps.h>
#include <base/objectmodel/entity.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <entities/nonguientities.h>

#include <base/memoryallocator/taggednew.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <cassert>

namespace ngs {

TestDeps::TestDeps(Entity* entity)
    : Component(entity, kIID(), kDID()),
      _factory(this),
      _mock(NULL),
      _dot_a(NULL),
      _dot_b(NULL),
      _dot_c(NULL),
      _dot_d(NULL),
      // Node Computes.
      _mock_compute(this),
      _dot_a_compute(this),
      _dot_b_compute(this),
      _dot_c_compute(this),
      _dot_d_compute(this),
      // Plug Computes.
      _mock_a_compute(this),
      _mock_b_compute(this),
      _mock_c_compute(this),
      _mock_d_compute(this),
      _dot_a_in_compute(this),
      _dot_a_out_compute(this),
      _dot_b_in_compute(this),
      _dot_b_out_compute(this),
      _dot_c_in_compute(this),
      _dot_c_out_compute(this),
      _dot_d_in_compute(this),
      _dot_d_out_compute(this){
  get_dep_loader()->register_fixed_dep(_factory, Path());
}

TestDeps::~TestDeps() {
}

void TestDeps::test() {
  set_up();
  test_connecting_plugs();
  test_cycle_detection();
  test_disconnecting_plugs();

  // Reconnect everything up again.
  test_connecting_plugs();
  test_collapsing_to_group();
  test_group_compute();
  test_saving();
  test_loading();
  test_dep_low_level();
}

void TestDeps::set_up() {
  Entity* root_group = get_entity(Path({"root"}));
  assert(root_group);

  // Create the nodes.
  _mock = _factory->instance_entity(root_group, EntityDID::kBaseMockNodeEntity, "mock");
  _dot_a = _factory->instance_entity(root_group, EntityDID::kBaseDotNodeEntity, "dot_a");
  _dot_b = _factory->instance_entity(root_group, EntityDID::kBaseDotNodeEntity, "dot_b");
  _dot_c = _factory->instance_entity(root_group, EntityDID::kBaseDotNodeEntity, "dot_c");
  _dot_d = _factory->instance_entity(root_group, EntityDID::kBaseDotNodeEntity, "dot_d");
  _mock->create_internals();
  _dot_a->create_internals();
  _dot_b->create_internals();
  _dot_c->create_internals();
  _dot_d->create_internals();

  // Initialize the wires.
  our_entity()->get_app_root()->initialize_wires();

  // Cache the compute components of the nodes.
  _mock_compute = get_dep<Compute>(_mock);
  _dot_a_compute = get_dep<Compute>(_dot_a);
  _dot_b_compute = get_dep<Compute>(_dot_b);
  _dot_c_compute = get_dep<Compute>(_dot_c);
  _dot_d_compute = get_dep<Compute>(_dot_d);

  // Cache the compute components of the plugs.
  _mock_a_compute = get_dep<InputCompute>(_mock->get_entity(Path({".",kInputsFolderName,"a"})));
  _mock_b_compute = get_dep<InputCompute>(_mock->get_entity(Path({".",kInputsFolderName,"b"})));
  _mock_c_compute = get_dep<OutputCompute>(_mock->get_entity(Path({".",kOutputsFolderName,"c"})));
  _mock_d_compute = get_dep<OutputCompute>(_mock->get_entity(Path({".",kOutputsFolderName,"d"})));

  _dot_a_in_compute = get_dep<InputCompute>(_dot_a->get_entity(Path({".",kInputsFolderName, Compute::kMainInputName})));
  _dot_a_out_compute = get_dep<OutputCompute>(_dot_a->get_entity(Path({".",kOutputsFolderName,Compute::kMainOutputName})));

  _dot_b_in_compute = get_dep<InputCompute>(_dot_b->get_entity(Path({".",kInputsFolderName,Compute::kMainInputName})));
  _dot_b_out_compute = get_dep<OutputCompute>(_dot_b->get_entity(Path({".",kOutputsFolderName,Compute::kMainOutputName})));

  _dot_c_in_compute = get_dep<InputCompute>(_dot_c->get_entity(Path({".",kInputsFolderName,Compute::kMainInputName})));
  _dot_c_out_compute = get_dep<OutputCompute>(_dot_c->get_entity(Path({".",kOutputsFolderName,Compute::kMainOutputName})));

  _dot_d_in_compute = get_dep<InputCompute>(_dot_d->get_entity(Path({".",kInputsFolderName,Compute::kMainInputName})));
  _dot_d_out_compute = get_dep<OutputCompute>(_dot_d->get_entity(Path({".",kOutputsFolderName,Compute::kMainOutputName})));

  // verify everything is dirty
  verify_all(true);
}

void TestDeps::test_connecting_plugs() {

  // Clean everything.
  clean_all_components();
  verify_all(false);

  // Connect dot a.
  _mock_a_compute->link_output_compute(_dot_a_out_compute);
  {
    std::unordered_set<Component*> cs;
    cs.insert(_mock_a_compute.get());
    cs.insert(_mock_compute.get());
    // mock c output param.
    cs.insert(_mock_c_compute.get());
    // mock d output param.
    cs.insert(_mock_d_compute.get());
    // Verify that these are dirty.
    verify(cs, true);
  }

  // Clean _mock_out_d.
  _mock_d_compute->clean_state();
  {
    // Only the mock c output param should be dirty.
    std::unordered_set<Component*> cs;
    cs.insert(_mock_c_compute.get());
    // Verity that these are dirty.
    verify(cs, true);
  }

  // Clean _mock_out_c.
  _mock_c_compute->clean_state();
  {
    // Everything should be clean now.
    verify_all(false);
  }

  // Connect dot c.
  _dot_c_in_compute->link_output_compute(_mock_c_compute);
  {
    std::unordered_set<Component*> cs;
    cs.insert(_dot_c_in_compute.get());
    cs.insert(_dot_c_compute.get());
    cs.insert(_dot_c_out_compute.get());
    // Verity that these are dirty.
    verify(cs, true);
  }

  // Clean the dot_c output plug.
  _dot_c_out_compute->clean_state();
  {
    // Everything should be clean now.
    verify_all(false);
  }

  // Connect dot d.
  _dot_d_in_compute->link_output_compute(_mock_d_compute);
  {
    std::unordered_set<Component*> cs;
    cs.insert(_dot_d_in_compute.get());
    cs.insert(_dot_d_compute.get());
    cs.insert(_dot_d_out_compute.get());
    // Verity that these are dirty.
    verify(cs, true);
  }

  // Clean the dot_d output plug.
  _dot_d_out_compute->clean_state();
  {
    // Everything should be clean now.
    verify_all(false);
  }

  // Connect dot b.
  _mock_b_compute->link_output_compute(_dot_b_out_compute);
  {
    std::unordered_set<Component*> cs;
    // Mock b input param.
    cs.insert(_mock_b_compute.get());

    // Mock node.
    cs.insert(_mock_compute.get());

    // Mock c chain.
    cs.insert(_mock_c_compute.get());
    cs.insert(_dot_c_in_compute.get());
    cs.insert(_dot_c_compute.get());
    cs.insert(_dot_c_out_compute.get());

    // Mock d chain.
    cs.insert(_mock_d_compute.get());
    cs.insert(_dot_d_in_compute.get());
    cs.insert(_dot_d_compute.get());
    cs.insert(_dot_d_out_compute.get());

    // Verity that these are dirty.
    verify(cs, true);
  }

  // Clean dot_c_out_out.
  _dot_c_out_compute->clean_state();
  {
    // Only the stuff down the mock d chain should be dirty.

    // Mock d chain.
    std::unordered_set<Component*> cs;
    cs.insert(_mock_d_compute.get());
    cs.insert(_dot_d_in_compute.get());
    cs.insert(_dot_d_compute.get());
    cs.insert(_dot_d_out_compute.get());

    // Verity that these are dirty.
    verify(cs, true);
  }

  // Clean dot_d_out_out.
  _dot_d_out_compute->clean_state();
  {
    // Everything should be clean now.
    verify_all(false);
  }
}

void TestDeps::test_cycle_detection() {
  assert(_dot_a_in_compute->can_link_output_compute(_dot_b_out_compute));
  assert(!_dot_a_in_compute->can_link_output_compute(_mock_c_compute));
  assert(!_dot_a_in_compute->can_link_output_compute(_mock_d_compute));
  assert(!_dot_a_in_compute->can_link_output_compute(_dot_c_out_compute));
  assert(!_dot_a_in_compute->can_link_output_compute(_dot_d_out_compute));

  assert(_dot_b_in_compute->can_link_output_compute(_dot_a_out_compute));
  assert(!_dot_b_in_compute->can_link_output_compute(_mock_c_compute));
  assert(!_dot_b_in_compute->can_link_output_compute(_mock_d_compute));
  assert(!_dot_b_in_compute->can_link_output_compute(_dot_c_out_compute));
  assert(!_dot_b_in_compute->can_link_output_compute(_dot_d_out_compute));
}

void TestDeps::test_disconnecting_plugs() {

  // Clean everything.
  clean_all_components();
  verify_all(false);

  // Disconnect mock_a_in.
  _mock_a_compute->unlink_output_compute();
  {
    std::unordered_set<Component*> cs;
    // DotA is clean.
    cs.insert(_dot_a_in_compute.get());
    cs.insert(_dot_a_compute.get());
    cs.insert(_dot_a_out_compute.get());
    // DotB is clean.
    cs.insert(_dot_b_in_compute.get());
    cs.insert(_dot_b_compute.get());
    cs.insert(_dot_b_out_compute.get());
    // MockB input param is clean.
    cs.insert(_mock_b_compute.get());
    // Verity that these are clean.
    verify(cs, false);
  }

  // Clean dot_c_out_compute.
  _dot_c_out_compute->clean_state();
  {
    std::unordered_set<Component*> cs;
    cs.insert(_mock_d_compute.get());
    cs.insert(_dot_d_in_compute.get());
    cs.insert(_dot_d_compute.get());
    cs.insert(_dot_d_out_compute.get());
    // Verity that these are dirty.
    verify(cs, true);
  }

  // Clean dot_d_out_compute.
  _dot_d_out_compute->clean_state();
  {
    // Everything should be clean now.
    verify_all(false);
  }

  // Disconnect mock_b_in.
  _mock_b_compute->unlink_output_compute();
  {
    std::unordered_set<Component*> cs;
    cs.insert(_mock_b_compute.get());
    cs.insert(_mock_compute.get());
    cs.insert(_mock_c_compute.get());
    cs.insert(_mock_d_compute.get());

    cs.insert(_dot_d_in_compute.get());
    cs.insert(_dot_d_compute.get());
    cs.insert(_dot_d_out_compute.get());

    cs.insert(_dot_c_in_compute.get());
    cs.insert(_dot_c_compute.get());
    cs.insert(_dot_c_out_compute.get());

    // Verity that these are dirty.
    verify(cs, true);
  }

  // Clean dot_c_out_compute.
  _dot_c_out_compute->clean_state();
  {
    std::unordered_set<Component*> cs;
    cs.insert(_mock_d_compute.get());
    cs.insert(_dot_d_in_compute.get());
    cs.insert(_dot_d_compute.get());
    cs.insert(_dot_d_out_compute.get());
    // Verity that these are dirty.
    verify(cs, true);
  }

  // Clean dot_d_out_compute.
  _dot_d_out_compute->clean_state();
  {
    // Everything should be clean now.
    verify_all(false);
  }

  // Disconnect dot_c_in_in.
  _dot_c_in_compute->unlink_output_compute();
  {
    std::unordered_set<Component*> cs;
    cs.insert(_dot_c_in_compute.get());
    cs.insert(_dot_c_compute.get());
    cs.insert(_dot_c_out_compute.get());
    // Verity that these are dirty.
    verify(cs, true);
  }

  // Clean dot_c_out_compute.
  _dot_c_out_compute->clean_state();
  {
    // Everything should be clean now.
    verify_all(false);
  }

  // Disconnect dot_d_in_in.
  _dot_d_in_compute->unlink_output_compute();
  {
    std::unordered_set<Component*> cs;
    cs.insert(_dot_d_in_compute.get());
    cs.insert(_dot_d_compute.get());
    cs.insert(_dot_d_out_compute.get());
    // Verify that these are dirty.
    verify(cs, true);
  }
}

void TestDeps::test_collapsing_to_group() {
}

void TestDeps::test_group_compute() {
}

void TestDeps::test_saving() {
}

void TestDeps::test_loading() {
}

void TestDeps::test_dep_low_level() {
}

void TestDeps::verify(std::unordered_set<Component*> cs, bool dirty) {
  // Node Computes.
  if (cs.count(_mock_compute.get())) {
    assert(_mock_compute->is_state_dirty() == dirty);
  } else {
    assert(_mock_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_dot_a_compute.get())) {
    assert(_dot_a_compute->is_state_dirty() == dirty);
  } else {
    assert(_dot_a_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_dot_b_compute.get())) {
    assert(_dot_b_compute->is_state_dirty() == dirty);
  } else {
    assert(_dot_b_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_dot_c_compute.get())) {
    assert(_dot_c_compute->is_state_dirty() == dirty);
  } else {
    assert(_dot_c_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_dot_d_compute.get())) {
    assert(_dot_d_compute->is_state_dirty() == dirty);
  } else {
    assert(_dot_d_compute->is_state_dirty() == !dirty);
  }

  // Plug Computes.
  if (cs.count(_mock_a_compute.get())) {
    assert(_mock_a_compute->is_state_dirty() == dirty);
  } else {
    assert(_mock_a_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_mock_b_compute.get())) {
    assert(_mock_b_compute->is_state_dirty() == dirty);
  } else {
    assert(_mock_b_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_mock_c_compute.get())) {
    assert(_mock_c_compute->is_state_dirty() == dirty);
  } else {
    assert(_mock_c_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_mock_d_compute.get())) {
    assert(_mock_d_compute->is_state_dirty() == dirty);
  } else {
    assert(_mock_d_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_dot_a_in_compute.get())) {
    assert(_dot_a_in_compute->is_state_dirty() == dirty);
  } else {
    assert(_dot_a_in_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_dot_a_out_compute.get())) {
    assert(_dot_a_out_compute->is_state_dirty() == dirty);
  } else {
    assert(_dot_a_out_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_dot_b_in_compute.get())) {
    assert(_dot_b_in_compute->is_state_dirty() == dirty);
  } else {
    assert(_dot_b_in_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_dot_b_out_compute.get())) {
    assert(_dot_b_out_compute->is_state_dirty() == dirty);
  } else {
    assert(_dot_b_out_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_dot_c_in_compute.get())) {
    assert(_dot_c_in_compute->is_state_dirty() == dirty);
  } else {
    assert(_dot_c_in_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_dot_c_out_compute.get())) {
    assert(_dot_c_out_compute->is_state_dirty() == dirty);
  } else {
    assert(_dot_c_out_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_dot_d_in_compute.get())) {
    assert(_dot_d_in_compute->is_state_dirty() == dirty);
  } else {
    assert(_dot_d_in_compute->is_state_dirty() == !dirty);
  }
  if (cs.count(_dot_d_out_compute.get())) {
    assert(_dot_d_out_compute->is_state_dirty() == dirty);
  } else {
    assert(_dot_d_out_compute->is_state_dirty() == !dirty);
  }
}

void TestDeps::verify_all(bool dirty) {
  std::unordered_set<Component*> cs;
  // Node Computes.
  cs.insert(_mock_compute.get());
  cs.insert(_dot_a_compute.get());
  cs.insert(_dot_b_compute.get());
  cs.insert(_dot_c_compute.get());
  cs.insert(_dot_d_compute.get());
  // Plug Computes.
  cs.insert(_mock_a_compute.get());
  cs.insert(_mock_b_compute.get());
  cs.insert(_mock_c_compute.get());
  cs.insert(_mock_d_compute.get());
  cs.insert(_dot_a_in_compute.get());
  cs.insert(_dot_a_out_compute.get());
  cs.insert(_dot_b_in_compute.get());
  cs.insert(_dot_b_out_compute.get());
  cs.insert(_dot_c_in_compute.get());
  cs.insert(_dot_c_out_compute.get());
  cs.insert(_dot_d_in_compute.get());
  cs.insert(_dot_d_out_compute.get());
  // Verify.
  verify(cs, dirty);
}


void TestDeps::clean_all_components() {
  // Node Computes.
  _mock_compute->clean_wires();
  _dot_a_compute->clean_wires();
  _dot_b_compute->clean_wires();
  _dot_c_compute->clean_wires();
  _dot_d_compute->clean_wires();

  // Plug Computes.
  _mock_a_compute->clean_wires();
  _mock_b_compute->clean_wires();
  _mock_c_compute->clean_wires();
  _mock_d_compute->clean_wires();
  _dot_a_in_compute->clean_wires();
  _dot_a_out_compute->clean_wires();
  _dot_b_in_compute->clean_wires();
  _dot_b_out_compute->clean_wires();
  _dot_c_in_compute->clean_wires();
  _dot_c_out_compute->clean_wires();
  _dot_d_in_compute->clean_wires();
  _dot_d_out_compute->clean_wires();

  // Node Computes.
  _mock_compute->clean_state();
  _dot_a_compute->clean_state();
  _dot_b_compute->clean_state();
  _dot_c_compute->clean_state();
  _dot_d_compute->clean_state();

  // Plug Computes.
  _mock_a_compute->clean_state();
  _mock_b_compute->clean_state();
  _mock_c_compute->clean_state();
  _mock_d_compute->clean_state();
  _dot_a_in_compute->clean_state();
  _dot_a_out_compute->clean_state();
  _dot_b_in_compute->clean_state();
  _dot_b_out_compute->clean_state();
  _dot_c_in_compute->clean_state();
  _dot_c_out_compute->clean_state();
  _dot_d_in_compute->clean_state();
  _dot_d_out_compute->clean_state();
}

void TestDeps::dirty_all_components() {
  // Dirty the compute components in the nodes.
  _mock_compute->dirty_state();
  _dot_a_compute->dirty_state();
  _dot_b_compute->dirty_state();
  _dot_c_compute->dirty_state();
  _dot_d_compute->dirty_state();

  // Dirty the compute components in the plugs
  _mock_a_compute->dirty_state();
  _mock_b_compute->dirty_state();
  _mock_c_compute->dirty_state();
  _mock_d_compute->dirty_state();
  _dot_a_in_compute->dirty_state();
  _dot_a_out_compute->dirty_state();
  _dot_b_in_compute->dirty_state();
  _dot_b_out_compute->dirty_state();
  _dot_c_in_compute->dirty_state();
  _dot_c_out_compute->dirty_state();
  _dot_d_in_compute->dirty_state();
  _dot_d_out_compute->dirty_state();
}


}
