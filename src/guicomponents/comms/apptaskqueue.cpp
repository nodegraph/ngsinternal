#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>
#include <base/objectmodel/basefactory.h>

#include <components/interactions/graphbuilder.h>
#include <components/computes/compute.h>
#include <components/computes/inputcompute.h>
#include <components/computes/outputcompute.h>
#include <components/computes/baseoutputs.h>
#include <components/computes/baseinputs.h>

#include <components/interactions/groupinteraction.h>

#include <components/compshapes/nodeshape.h>
#include <components/compshapes/linkshape.h>
#include <components/compshapes/inputshape.h>
#include <components/compshapes/outputshape.h>
#include <components/compshapes/compshapecollective.h>

#include <guicomponents/comms/apptaskqueue.h>
#include <guicomponents/comms/messagesender.h>

#include <entities/entityids.h>

namespace ngs {

AppTaskQueue::AppTaskQueue(Entity* parent)
    : QObject(NULL),
      Component(parent, kIID(), kDID()),
      _msg_sender(this),
      _waiting_for_response(false),
      _connected(false) {
  get_dep_loader()->register_fixed_dep(_msg_sender, Path({}));
}

AppTaskQueue::~AppTaskQueue() {
}

void AppTaskQueue::open() {
  external();
  _msg_sender->open();
}

void AppTaskQueue::close() {
  external();
  _msg_sender->close();
}

bool AppTaskQueue::is_open() const {
  external();
  return _msg_sender->is_open();
}

void AppTaskQueue::set_empty_stack_callback(std::function<void()> callback) {
  external();
  _empty_stack_callback = callback;
}

// ---------------------------------------------------------------------------------
// Queue Framework Tasks.
// ---------------------------------------------------------------------------------

void AppTaskQueue::queue_start_sequence(AppTaskContext& tc) {
  tc.stack_index = _stack.size();
  _stack.push_back(TaskQueue());
  queue_task(tc, (AppTask)std::bind(&AppTaskQueue::start_sequence_task,this), "queue_start_sequence");
}

void AppTaskQueue::queue_finished_sequence(AppTaskContext& tc, std::function<void()> on_finished_sequence) {
  queue_task(tc, (AppTask)std::bind(&AppTaskQueue::finished_sequence_task,this,on_finished_sequence), "queue_finished_sequence");
}

// ------------------------------------------------------------------------
// Task Queue Management.
// ------------------------------------------------------------------------

AppTaskQueue::TaskQueue& AppTaskQueue::get_top_queue() {
  return _stack.back();
}

void AppTaskQueue::queue_task(AppTaskContext& tc, AppTask task, const std::string& about) {

  // Check to see if we can run the next worker.
  bool ok_to_run = true;

  // Make sure we're not already waiting for results.
  // We only allow one request to be in-flight at a time.
  if (_waiting_for_response) {
    ok_to_run = false;
  }

  if (get_top_queue().size() > 0) {
    ok_to_run = false;
  }

  // Push the task onto the right queue for the context. (This is not always the top queue.)
  // This is important because while a TaskContext is open another function/routine may open
  // another Sub-TaskContext. When the Sub-TaskContext falls out of context, we will be left
  // with its queue at the top of the queue stack. Now when other tasks get queued from the
  // original task context, the top queue is not the right one for it. The top one used to be
  // the right one for it before the Sub-TaskContext was created and destroyed. This is why
  // we have to the select the appropriate queue with tc.stack_index.
  _stack[tc.stack_index].push_back(task);

  // Run the worker if we're all clear to run.
  if (ok_to_run) {
    run_next_task();
  }
}

void AppTaskQueue::run_next_task() {
  // If we're waiting for a response, then don't run the next task yet.
  if (_waiting_for_response) {
    return;
  }

  if (_stack.empty()) {
    _empty_stack_callback();
    return;
  }

  // If the queue is empty we're done.
  if (get_top_queue().empty()) {
    return;
  }

  // Pop a task and run it..
  AppTask task = get_top_queue().front();
  get_top_queue().pop_front();
  task();
}

// ------------------------------------------------------------------------
// Incoming Message Handlers.
// ------------------------------------------------------------------------

void AppTaskQueue::handle_response(const Message& msg) {
  // Get the response id. This is supposed to match with the request id.
  int resp_id = msg[Message::kID].toInt();

  // Determine the request id.
  int req_id = _next_msg_id -1;

  // Check the ids.
  if (resp_id != req_id) {
    std::cerr << "Error: response id: " << resp_id << " did not match request id: " << req_id << "\n";
  } else {
    std::cerr << "Success: response id: " << resp_id << " matches request id: " << req_id << "\n";
  }

  assert(resp_id == req_id);

  // Update the last message.
  _last_response = msg;

  // Record the fact that we've received our response.
  _waiting_for_response = false;

  // Run the next task.
  run_next_task();
}

void AppTaskQueue::handle_info(const Message& msg) {
}

// ------------------------------------------------------------------------
// Infrastructure Tasks.
// ------------------------------------------------------------------------

void AppTaskQueue::send_msg_task(Message msg) {
  // Tag the request with an id. We expect a response with the same id.
  msg[Message::kID] = _next_msg_id;

  std::cerr << "app --> comhub: " << msg.to_string().toStdString() << "\n";

  // Increment the counter. Note we're ok with this overflowing and looping around.
  _next_msg_id += 1;

  // Record the fact that we are now waiting for a response.
  _waiting_for_response = true;

  // Send the request to nodejs.
  _msg_sender->send_msg(msg);
}

void AppTaskQueue::start_sequence_task() {
  // Make sure that we are the first task in the queue.
  //assert(get_task_queue().empty());
  run_next_task();
}

void AppTaskQueue::finished_sequence_task(std::function<void()> on_finished_sequence) {
  // Make sure that we are the last task in the queue.
  assert(get_top_queue().empty());

  // We're done with this sequence of tasks so we pop the queue off of the stack.
  // Note each sequence of tasks is associated with a TaskContext and with one queue on the stack.
  _stack.pop_back();

  // Now call the callback which may add more queues to the stack.
  if (on_finished_sequence) {
    on_finished_sequence();
  }

  run_next_task();
}

}
