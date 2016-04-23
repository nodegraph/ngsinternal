#include <base/utils/signal.h>
#include <iostream>

// Just a private namespace to test the signals and slots.
namespace {

using namespace ngs;

void test(int x) {
  std::cerr << "received the number: " << x << "\n";
}

Slot<int> slot(std::function<void(int)>(test));

Signal<int> signal;

Connection<int>* connection = signal.connect(std::function<void(int)>(test));

void hello() {
  signal(123);
}

}
