#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <base/objectmodel/deploader.h>
#include <guicomponents/comms/guitypes.h>
#include <guicomponents/computes/waitnodecompute.h>

#include <guicomponents/quick/basenodegraphmanipulator.h>

namespace ngs {

WaitNodeCompute::WaitNodeCompute(Entity* entity)
    : QObject(),
      Compute(entity, kDID()),
      _restart_timer(true) {
  // Setup the wait timer.
  _wait_timer.setSingleShot(false);
  _wait_timer.setInterval(1000);
  connect(&_wait_timer,SIGNAL(timeout()),this,SLOT(on_done_wait()));
}

WaitNodeCompute::~WaitNodeCompute() {
}

void WaitNodeCompute::create_inputs_outputs(const EntityConfig& config) {
  external();
  Compute::create_inputs_outputs(config);
  create_main_input(config);
  create_main_output(config);

  {
    EntityConfig c = config;
    c.expose_plug = false;
    c.unconnected_value = 5000;
    create_input("time_in_milliseconds", c);
  }
}

const QJsonObject WaitNodeCompute::_hints = WaitNodeCompute::init_hints();
QJsonObject WaitNodeCompute::init_hints() {
  QJsonObject m;
  add_main_input_hint(m);
  add_hint(m, "time_in_milliseconds", GUITypes::HintKey::DescriptionHint, "The time to wait specified in milliseconds.");
  return m;
}

bool WaitNodeCompute::update_state() {
  internal();
  Compute::update_state();
  set_main_output(_inputs->get_main_input_object());

  // Timer logic.
  if (_restart_timer) {
    double time = _inputs->get_input_double("time_in_milliseconds");
    _wait_timer.start(time);
    _restart_timer = false;
  }
  if (_wait_timer.isActive()) {
    return false;
  }
  return true;
}

void WaitNodeCompute::set_self_dirty(bool dirty) {
  Compute::set_self_dirty(dirty);
  _restart_timer = true;
}

void WaitNodeCompute::init_dirty_state() {
  Compute::init_dirty_state();
  _restart_timer = true;
}

void WaitNodeCompute::on_done_wait() {
  _wait_timer.stop();
  _manipulator->continue_cleaning_to_ultimate_targets_on_idle();
}

}
