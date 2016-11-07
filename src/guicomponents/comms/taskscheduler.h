#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>

#include <functional>
#include <deque>

class QString;

namespace ngs {

class MessageSender;
class BaseNodeGraphManipulator;

class TaskContext;
class Message;

typedef std::function<void()> Task;

class COMMS_EXPORT TaskScheduler : public Component {
 public:
  COMPONENT_ID(TaskScheduler, TaskScheduler)

  explicit TaskScheduler(Entity* parent);
  virtual ~TaskScheduler();

  bool is_waiting_for_response() const {return _waiting_for_response;}

  void force_stack_reset();

  void queue_task(TaskContext& tc, Task task, const std::string& about);
  void run_next_task();

  // Pause the queue to allow our users to wait for some type of asynchronous compute to return.
  int wait_for_response();
  void done_waiting_for_response(int resp_id, const QString& error);

  // Task queue info.
  bool is_busy() const {external(); return !_stack.empty();}

 private:
  typedef std::deque<Task> Queue;
  typedef std::vector<Queue> QueueStack;

  // Task Management.
  Queue& get_top_queue();

  // Methods to queue our special start and finish tasks.
  void queue_start_sequence(TaskContext& tc);
  void queue_finished_sequence(TaskContext& tc);

  // Our special start and finish tasks.
  void start_sequence_task();
  void finished_sequence_task();

  // Our fixed dependencies.
  Dep<BaseNodeGraphManipulator> _manipulator;

  // A stack of queues.
  QueueStack _stack;

  // Simple Request-Response Pair Tracking.
  bool _waiting_for_response;
  int _next_msg_id;  // This is one more than the max msg id issued out.

  // Used after resetting the queue, as the response for a request with this id may still be outstanding.
  bool _ignore_outstanding_response;
  int _outstanding_response_id;

  // Whether we're connected for receiving text messages from the socket.
  bool _connected;

  // Friends.
  friend class TaskContext;
};


class COMMS_EXPORT TaskContext {
 public:
  TaskContext(Dep<TaskScheduler>& task_queue):
    task_queue(task_queue.get()),
    stack_index(-1) {
    task_queue->queue_start_sequence(*this);
  }
  ~TaskContext(){
    task_queue->queue_finished_sequence(*this);
  }
  TaskScheduler* task_queue;
  size_t stack_index;
};

}

