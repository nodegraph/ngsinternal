#include <base/utils/stringutil.h>
#include <base/utils/drawutil.h>
#include <base/utils/simplesaver.h>
#include <base/utils/simpleloader.h>
#include <base/utils/signal.h>

#include <base/memoryallocator/taggednew.h>
#include <base/memoryallocator/bootstrap.h>
#include <base/utils/fileutil.h>
#include <base/utils/envutil.h>

#include <iostream>
#include <sstream>
#include <fstream>

namespace ngs {

// draw utils
void test_circle_samples();

// env utils
void test_get_env();

// string utils
void test_split_by_char();
void test_split_by_regex();
void test_replace_by_regex();

// serialization
void test_simple_serialization_binary();

// signals
void test_signals();

}

// Main routine.
int main(int argc, char **argv) {
  using namespace ngs;

  bootstrap_memory_tracker();
  {
    // draw utils
    test_circle_samples();

    // env utils.
    test_get_env();

    // string utils.
    test_split_by_char();
    test_split_by_regex();
    test_replace_by_regex();

    // serialization.
    test_simple_serialization_binary();

    // signals.
    test_signals();
  }
  shutdown_memory_tracker();

  return 0;
}

namespace ngs {

// draw utils

void test_circle_samples() {
  std::vector<float> cos_samples;
  std::vector<float> sin_samples;

  get_cos_samples(24, cos_samples);
  get_sin_samples(24, sin_samples);
}

void test_get_env() {
  std::string env_value;

  // Test getting a variable that exists.
  env_value = get_env("PATH");
  assert(env_value != "");

  // Test getting a variable that doesn't exist.
  env_value = get_env("lkjweoirukldfs");
  assert(env_value == "");
}

void test_split_by_char() {
  std::string src = "&*hello***&*-my--name**-&&--&is&&*booya*&&&";
  std::vector<std::string> delimiters = {"&","*","-"};
  std::vector<std::string> results = split(src, delimiters);
  assert(results.size() == 5);
  assert(results[0] == "hello");
  assert(results[1] == "my");
  assert(results[2] == "name");
  assert(results[3] == "is");
  assert(results[4] == "booya");
}

void test_split_by_regex() {
  // Test splitting by a regex string.
  {
    std::string src = "&*hello***&*-my--name**-&&--&is&&*booya*&&&";
    std::string expr = "[&*-]+";
    std::vector<std::string> results;
    results = split_by_expr(src, expr);

    assert(results.size() == 5);
    assert(results[0] == "hello");
    assert(results[1] == "my");
    assert(results[2] == "name");
    assert(results[3] == "is");
    assert(results[4] == "booya");
  }

  // Test splitting by a *non-regex* regex string.
  {
    std::string src =
        "xc*rpxxc*rpxhelloxc*rpxmyxc*rpxxc*rpxnamexc*rpxisxc*rpxbooyaxc*rpx";
    std::string expr = "xc\\*rpx";
    std::vector<std::string> results;
    results = split_by_expr(src, expr);

    assert(results.size() == 5);
    assert(results[0] == "hello");
    assert(results[1] == "my");
    assert(results[2] == "name");
    assert(results[3] == "is");
    assert(results[4] == "booya");
  }

  // Test splitting with no resulting splits.
  {
    std::string src = "hellomynameisbooya";
    std::string expr = "xc\\*rpx";
    std::vector<std::string> results;
    results = split_by_expr(src, expr);

    assert(results.size() == 1);
    assert(results[0] == "hellomynameisbooya");

  }
}

void test_replace_by_regex() {
  // Test splitting by a *non-regex* regex string.
  {
    std::string src =
        "xc*rpxxc*rpxhelloxc*rpxmyxc*rpxxc*rpxnamexc*rpxisxc*rpxbooyaxc*rpx";
    std::string expr = "xc\\*rpx";
    std::string result = replace_by_regex(src, expr, "-");
    assert(result == "--hello-my--name-is-booya-");
  }
}

void test_simple_serialization_binary() {

  std::string filename = get_env("SRC_ROOT")+"/serializationtest.bin";

  // --------------------------------------------------------------------------
  // Test writing to binary file.
  // --------------------------------------------------------------------------
  {
    std::ofstream ofs;
    open_output_file_stream(filename, ofs);
    SimpleSaver ss(ofs);
    int a = 5;
    float b = 9.87654321f;
    short c = 16;
    std::string d = "123hello there ";
    std::string e = "  i am booya  ";
    float f = 1.2345f;
    long g = -987654321;
    unsigned long h = 987654321;
    ss.save(a);
    ss.save(b);
    ss.save(c);
    ss.save(d);
    ss.save(e);
    ss.save(f);
    ss.save(g);
    ss.save(h);
  }

  // --------------------------------------------------------------------------
  // Test reading from a binary file.
  // --------------------------------------------------------------------------

  {
    std::ifstream ifs;
    open_input_file_stream(filename, ifs);
    Bits* bits = create_bits_from_input_stream(ifs);
    SimpleLoader sl(bits);

    // Set the values to different values, to make sure we're reading in new_ values.
    int a = 0;
    float b = 0;
    short c = 0;
    std::string d = "";
    std::string e = "";
    float f = 0;
    long g = 0;
    unsigned long h = 0;

    // Read in from the binary file.
    sl.load(a);
    sl.load(b);
    sl.load(c);
    sl.load(d);
    sl.load(e);
    sl.load(f);
    sl.load(g);
    sl.load(h);

    // Check to make sure it matches what we wrote in.
    assert(a == 5);
    assert(b == 9.87654321f);
    assert(c == 16);
    assert(d == "123hello there ");
    assert(e == "  i am booya  ");
    assert(f == 1.2345f);
    assert(g == -987654321);
    assert(h == 987654321);
  }
}

int sum = 0;
void callback1(int x) {
  sum+=x;
}
void callback2(int x) {
  sum*=x;
}

void test_signals() {

  {
    Signal<int> signal;

    // Test connections going out of scope.
    {
      Connection<int>* connection1 = signal.connect(std::function<void(int)>(callback1));
      Connection<int>* connection2 = signal.connect(std::function<void(int)>(callback2));

      // Fire the signal.
      signal(3);
      assert(sum == 9);
      signal(2);
      assert(sum == 22);

      // Disconnect the signals.
      connection2->disconnect();

      // Fire the signal.
      signal(5);
      assert(sum == 27);

      delete_ff(connection1);
      delete_ff(connection2);
    }

    // Make sure the signal has no connections.
    assert(signal.get_num_connections()==0);

    // Make sure the sum is not modified when we emit a signal.
    signal(999);
    assert(sum == 27);
  }

  // Test Signal getting deleted first.
  {
    Signal<int>* signal = new_ff Signal<int>();
    Connection<int>* connection1 = signal->connect(std::function<void(int)>(callback1));
    Connection<int>* connection2 = signal->connect(std::function<void(int)>(callback2));

    // Emit a signal.
    (*signal)(2);
    assert(sum == 58);

    // Delete the signal.
    delete_ff(signal);

    // Make sure the connection have null signals and slots.
    assert(connection1->_signal==NULL);
    assert(connection1->_slot==NULL);
    assert(connection2->_signal==NULL);
    assert(connection2->_slot==NULL);

    delete_ff(connection1);
    delete_ff(connection2);

  }


}

}

