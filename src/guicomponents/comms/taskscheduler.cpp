#include <base/memoryallocator/taggednew.h>
#include <base/objectmodel/deploader.h>

#include <guicomponents/comms/taskscheduler.h>
#include <guicomponents/comms/messagesender.h>
#include <guicomponents/quick/basenodegraphmanipulator.h>

#include <QtCore/QCoreApplication>

namespace ngs {

TaskScheduler::TaskScheduler(Entity* parent)
    : Component(parent, kIID(), kDID()),
      _msg_sender(this),
      _ng_manipulator(this),
      _waiting_for_response(false),
      _next_msg_id(0),
      _ignore_outstanding_response(false),
      _outstanding_response_id(-1),
      _connected(false) {
  get_dep_loader()->register_fixed_dep(_msg_sender, Path({}));
  get_dep_loader()->register_fixed_dep(_ng_manipulator, Path({}));
}

TaskScheduler::~TaskScheduler() {
}

void TaskScheduler::open() {
  external();
  _msg_sender->open();
}

void TaskScheduler::close() {
  external();
  _msg_sender->close();
}

bool TaskScheduler::is_open() const {
  external();
  return _msg_sender->is_open();
}

void TaskScheduler::force_stack_reset() {
  std::cerr << "forcing stack reset \n";

  // Clear the stack of queues.
  _stack.clear();

  // An outstanding response to be sent back to us later.
  // Record state to ignore it.
  _ignore_outstanding_response = true;
  _outstanding_response_id = _next_msg_id -1;

  // Now reset our other members.
  _waiting_for_response = false;
  _next_msg_id = 0;
  _last_response = Message();
}

// ---------------------------------------------------------------------------------
// Queue Framework Tasks.
// ---------------------------------------------------------------------------------

void TaskScheduler::queue_start_sequence(TaskContext& tc) {
  tc.stack_index = _stack.size();
  _stack.push_back(Queue());
  queue_task(tc, (Task)std::bind(&TaskScheduler::start_sequence_task,this), "queue_start_sequence");
}

void TaskScheduler::queue_finished_sequence(TaskContext& tc) {
  queue_task(tc, (Task)std::bind(&TaskScheduler::finished_sequence_task,this), "queue_finished_sequence");
}

// ------------------------------------------------------------------------
// Task Queue Management.
// ------------------------------------------------------------------------

TaskScheduler::Queue& TaskScheduler::get_top_queue() {
  return _stack.back();
}

void TaskScheduler::queue_task(TaskContext& tc, Task task, const std::string& about) {

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

void TaskScheduler::run_next_task() {
  // If we're waiting for a response, then don't run the next task yet.
  if (_waiting_for_response) {
    return;
  }

  if (_stack.empty()) {
    _ng_manipulator->continue_cleaning_to_ultimate_target();
    return;
  }

  // If the queue is empty we're done.
  if (get_top_queue().empty()) {
    return;
  }

  // Pop a task and run it..
  Task task = get_top_queue().front();
  get_top_queue().pop_front();
  task();
}

// ------------------------------------------------------------------------
// Incoming Message Handlers.
// ------------------------------------------------------------------------

void TaskScheduler::handle_response(const Message& msg) {
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

  if (resp_id != req_id) {
    if (_ignore_outstanding_response && resp_id == _outstanding_response_id) {
      _ignore_outstanding_response = false;
      _outstanding_response_id = -1;
      return;
    } else {
      assert(false);
    }
  }

  // Update the last message.
  _last_response = msg;

  // Record the fact that we've received our response.
  _waiting_for_response = false;

  // If the response indicates an un-continuable error has occured, we reset the stack.
  if (!_last_response[Message::kSuccess].toBool()) {
    force_stack_reset();
    // Also show the error marker on the node.
    _ng_manipulator->set_error_node();
    _ng_manipulator->clear_ultimate_target();
  }

  // Run the next task.
  run_next_task();
}

void TaskScheduler::handle_info(const Message& msg) {
}

// ------------------------------------------------------------------------
// Infrastructure Tasks.
// ------------------------------------------------------------------------

void TaskScheduler::send_msg_task(Message msg) {
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

void TaskScheduler::start_sequence_task() {
  // Make sure that we are the first task in the queue.
  //assert(get_task_queue().empty());
  run_next_task();
}

void TaskScheduler::finished_sequence_task() {
  // Make sure that we are the last task in the queue.
  assert(get_top_queue().empty());

  // We're done with this sequence of tasks so we pop the queue off of the stack.
  // Note each sequence of tasks is associated with a TaskContext and with one queue on the stack.
  _stack.pop_back();

  // Run the next task.
  run_next_task();
}

}
