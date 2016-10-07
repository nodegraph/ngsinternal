#pragma once
#include <guicomponents/comms/comms_export.h>
#include <base/objectmodel/component.h>
#include <entities/componentids.h>
#include <base/objectmodel/dep.h>
#include <guicomponents/comms/message.h>

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtNetwork/QAbstractSocket>
#include <QtNetwork/QSslError>

#include <functional>
#include <deque>

namespace ngs {

class MessageSender;
class FileModel;
class GraphBuilder;
class Compute;
class BaseFactory;
class NodeSelection;

class TaskContext;

typedef std::function<void()> Task;

class COMMS_EXPORT TaskScheduler : public Component {
 public:
  COMPONENT_ID(TaskScheduler, TaskScheduler)

  explicit TaskScheduler(Entity* parent);
  virtual ~TaskScheduler();

  void open();
  void close();
  bool is_open() const;
  bool is_waiting_for_response() const {return _waiting_for_response;}

  void reset_stack();
  void set_empty_stack_callback(std::function<void()> callback);

  void queue_task(TaskContext& tc, Task task, const std::string& about);
  void run_next_task();
  void send_msg_task(Message msg);

  // Task queue info.
  bool is_busy() const {external(); return !_stack.empty();}
  const Message& get_last_response() const{external(); return _last_response;}

  // Handle incoming messages.
  void handle_response(const Message& sm);
  void handle_info(const Message& msg);

 private:
  typedef std::deque<Task> Queue;
  typedef std::vector<Queue> QueueStack;

  // Task Management.
  Queue& get_top_queue();

  // Methods to queue our special start and finish tasks.
  void queue_start_sequence(TaskContext& tc);
  void queue_finished_sequence(TaskContext& tc, std::function<void()> on_finished_sequence);

  // Our special start and finish tasks.
  void start_sequence_task();
  void finished_sequence_task(std::function<void()> on_finished_sequence);

  // Our fixed dependencies.
  Dep<MessageSender> _msg_sender;

  // Called when the stack becomes empty.
  std::function<void()> _empty_stack_callback;

  // A stack of queues.
  QueueStack _stack;

  // Simple Request-Response Pair Tracking.
  bool _waiting_for_response;
  int _next_msg_id;  // This is one more than the max msg id issued out.
  int _expected_msg_id; // This is the next msg id we're expecting from a response message.
  Message _last_response; // The last response message received.

  // Whether we're connected for receiving text messages from the socket.
  bool _connected;

  // Friends.
  friend class TaskContext;
};


class COMMS_EXPORT TaskContext {
 public:
  TaskContext(Dep<TaskScheduler>& task_queue, std::function<void ()> on_finished_sequence = std::function<void()>()):
    task_queue(task_queue.get()),
    on_finished_sequence(on_finished_sequence),
    stack_index(-1) {
    task_queue->queue_start_sequence(*this);
  }
  ~TaskContext(){
    task_queue->queue_finished_sequence(*this,on_finished_sequence);
  }
  TaskScheduler* task_queue;
  std::function<void ()> on_finished_sequence;
  size_t stack_index;
};

}

