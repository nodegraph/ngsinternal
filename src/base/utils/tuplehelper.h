#pragma once
#include <base/utils/utils_export.h>

#include<tuple>
#include<type_traits>
#include<string>
#include<iostream>


// -------------------------------------------------------------------------
// Helper functions for std::tuple.
// -------------------------------------------------------------------------

// Base case with no types.
template <class SearchType, std::size_t Count, class... Types>
struct get_tuple_index_by_type
{
    static const std::size_t index = Count; // In C++1 use constexpr instead.
};

// Base case where the search type is found.
template <class SearchType, std::size_t Count, class... TailTypes>
struct get_tuple_index_by_type<SearchType, Count, SearchType, TailTypes...>
{
    static const std::size_t index = Count; // In C++1 use constexpr instead.
};

// Recursive case where the search type is not yet found.
template <class SearchType, std::size_t Count, class HeadTypes, class... TailTypes>
struct get_tuple_index_by_type<SearchType, Count, HeadTypes, TailTypes...>
{
    static const std::size_t index = get_tuple_index_by_type<SearchType, Count + 1, TailTypes...>::index; // In C++1 use constexpr instead.
};

// Get element from a tuple by type.
template <class SearchType, class... Types>
SearchType& get_element_by_type(std::tuple<Types...>& t)
{
    return std::get<get_tuple_index_by_type<SearchType, 0, Types...>::index>(t);
}
