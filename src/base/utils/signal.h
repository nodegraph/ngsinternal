#pragma once
#include <base/memoryallocator/taggednew.h>
#include <functional>
#include <list>
#include <algorithm>
#include <iostream>

namespace ngs {

template<class ... F>
class Connection;

template<class ... F>
class Signal;

// All our callbacks should return void, because we don't implement
// any kind of collection logic to collect all the return values.
template<class... F>
class Slot {
 public:
    Slot(const std::function<void(F...)>& callback):
      _callback(callback),
      _enabled(true),
      _connection(NULL){}

    // Slots can be immediately disabled.
    // This disables the slot immediately even when we're in some nested
    // slot calling situation.
    // Disabled slots will be cleared the next time the signal is emitted,
    // at the top level. ie not nested
    void disable() {
      _enabled = false;
    }
    bool is_enabled() {
      return _enabled;
    }

    void call(F ... args) {
      _callback(args...);
    }

 private:
    std::function<void(F...)> _callback;
    bool _enabled;
    Connection<F...>* _connection; // borrowed reference

    friend class Signal<F...>;
    friend class Connection<F...>;
};


template<class ... F>
class Signal {
 public:
  Signal()
      : _recursion_level(0) {
  }
  ~Signal() {
    for (Slot<F...>* item : _slots) {
      item->disable();
    }
    erase_disconnected();
  }
  void operator()(F ... args) {
    if (_recursion_level == 0) {
      erase_disconnected();
    }
    _recursion_level++;
    for (Slot<F...>* slot : _slots) {
      slot->call(args...);
    }
    _recursion_level--;
  }
  Connection<F...>* connect(const std::function<void(F...)>& callback) {
    Slot<F...>* slot = new_ff Slot<F...>(callback);
    _slots.push_back(slot);
    return new_ff Connection<F...>(this, slot);
  }
  void disconnect_all() {
    for (Slot<F...>* slot : _slots) {
      slot->Disable();
    }
  }
  size_t get_num_connections() {
    size_t enabled_count=0;
    for (Slot<F...>* slot : _slots) {
      if (slot->is_enabled()) {
        ++enabled_count;
      }
    }
    return enabled_count;
  }
 private:
  std::list<Slot<F...>*> _slots; // We own these slots.
  unsigned _recursion_level;

  void erase_disconnected() {
    _slots.erase(std::remove_if(_slots.begin(), _slots.end(), [](Slot<F...>* slot) {
      if (!slot->is_enabled()) {
        slot->_connection->_slot=NULL;
        slot->_connection->_signal=NULL;
        delete_ff(slot);
        return true;
      }
      return false;}),
                 _slots.end());
  }
};

template<class ... F>
class Connection {
 public:
  Connection(Signal<F...>* signal, Slot<F...>* slot)
      : _signal(signal),
        _slot(slot) {
    slot->_connection = this;
  }
  Connection()
      : _signal(NULL),
        _slot(NULL) {
  }
  ~Connection() {
    disconnect();
  }
  void operator=(const Connection&) = delete;
  void disconnect(){if (_slot) {_slot->disable();}};
 private:
  // Note that when the Signal object is deleted it will nullify these.
  Signal<F...>* _signal;
  Slot<F...>* _slot;

  friend class Signal<F...>;
  friend void test_signals();
};


}


