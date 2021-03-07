#pragma once

#include <unordered_set>
#include <unordered_map>

namespace ngs {

template <class T>
class Dep;

template <class T>
struct DepHash;

template<class T>
using DepUSet=std::unordered_set<Dep<T>, DepHash<T> >;

template<class T, class T2>
using DepUnorderedMap=std::unordered_map<Dep<T>, T2, DepHash<T> >;

}
