//#pragma once
//#include <base/utils/utils_export.h>
//
//#include <cassert>
//#include <thread>
//
//namespace ngs {
//
//class ThreadPool  {
//public:
//
//  static inline std::vector<std::pair<size_t, size_t> > get_task_ranges(size_t total, size_t num_threads) {
//    std::vector<std::pair<size_t, size_t> > ranges;
//    if (total <= num_threads) {
//      for (size_t i=0; i<total; ++i) {
//        ranges.push_back({i,i+1});
//      }
//      std::cerr << "return range of size: " << ranges.size() << "\n";
//      return ranges;
//    }
//
//    // Otherwise.
//    size_t div = total / num_threads;
//    size_t mod = total % num_threads;
//    if (mod > 0) {
//      div += 1;
//    }
//    for (size_t i=0; i<num_threads; ++i) {
//      size_t begin = i * div;
//      size_t end = (i+1) * div;
//      if (i == num_threads-1) {
//        end = total;
//      }
//      ranges.push_back({begin,end});
//    }
//    std::cerr << "return range of size2: " << ranges.size() << "\n";
//    return ranges;
//  }
//
//  ThreadPool(size_t num_elements,
//             size_t num_threads,
//             std::function<void(std::pair<size_t, size_t>&)>& func)
//      : _num_elements(num_elements),
//        _num_threads(num_threads) {
//    _ranges = get_task_ranges(num_elements, _num_threads);
//    for (size_t i=0; i<_ranges.size(); ++i) {
//      _threads.push_back(std::thread(func, _ranges[i]));
//    }
//    join();
//  }
//
//  ~ThreadPool() {
//  }
//
//  void join() {
//    for (size_t i=0; i<_threads.size(); ++i) {
//      _threads[i].join();
//    }
//  }
//
// private:
//  size_t _num_elements;
//  size_t _num_threads;
//  std::vector<std::pair<size_t, size_t> > _ranges;
//  std::vector<std::thread> _threads;
//};
//
//
//
//}
