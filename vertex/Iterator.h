#pragma once
#include <iterator>

namespace toolbox {

/** Get the last value of an iterator within the given closed interval */
template <typename Iterator>
Iterator back(const Iterator& begin, const Iterator& end);

/** Get the last element in a container */
template <typename Container>
auto back(const Container& value);

/** Get the last value of an iterator within the given open interval */
template <typename Iterator>
Iterator end(const Iterator& begin, const Iterator& end);

/********************************IMPLEMENTATION********************************/

template <typename Iterator>
Iterator back(const Iterator& begin, const Iterator& end) {
  auto result = begin;
  for (auto it = begin; it != end; ++it) {
    result = it;
  }
  return result;
}

template <typename Container>
auto back(const Container& value) {
  return back<decltype(std::begin(value))>(std::begin(value), std::end(value));
}

template <typename Iterator>
Iterator end(const Iterator& begin, const Iterator& end) {
  auto result = begin;
  while (result != end) {
    ++result;
  }
  return result;
}

}  // namespace toolbox
