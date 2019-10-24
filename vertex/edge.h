#pragma once

#include <array>

namespace vertex {

template <typename Container>
class edge {
 public:
  using value_type = typename Container::key_type;
  using underlying_type = std::array<value_type, 2>;
  using iterator = typename underlying_type::iterator;
  using const_iterator = typename underlying_type::const_iterator;

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;

  /** Creates an empty edge */
  edge() = default;

  /** Creates an edge from two links */
  edge(value_type source, value_type target);

  /** Determine whether the edge links to a root Node
   * A root edge has no source */
  [[nodiscard]] bool is_root() const;

  /** Determine whether the edge links to a leaf Node
   * A leaf edge has no target */
  [[nodiscard]] bool is_leaf() const;

  /** Get the source Node */
  const value_type& source() const;

  /** Get the target Node */
  const value_type& target() const;

  static edge root(value_type target);

  static edge leaf(value_type source);

 private:
  underlying_type data_;
};

template <typename Container>
edge<Container>::edge(value_type source, value_type target)
    : data_{std::move(source), std::move(target)} {}

template <typename Container>
const typename edge<Container>::value_type& edge<Container>::source() const {
  return data_[0];
}

template <typename Container>
const typename edge<Container>::value_type& edge<Container>::target() const {
  return data_[1];
}

template <typename Container>
typename edge<Container>::iterator edge<Container>::begin() {
  return data_.begin();
}

template <typename Container>
typename edge<Container>::const_iterator edge<Container>::begin() const {
  return data_.begin();
}

template <typename Container>
typename edge<Container>::iterator edge<Container>::end() {
  return data_.end();
}

template <typename Container>
typename edge<Container>::const_iterator edge<Container>::end() const {
  return data_.end();
}

template <typename Container>
typename edge<Container>::const_iterator edge<Container>::cbegin() const {
  return data_.cbegin();
}

template <typename Container>
typename edge<Container>::const_iterator edge<Container>::cend() const {
  return data_.cend();
}

template <typename Container>
bool edge<Container>::is_root() const {
  return source() == value_type{};
}

template <typename Container>
bool edge<Container>::is_leaf() const {
  return target() == value_type{};
}

template <typename Container>
edge<Container> edge<Container>::root(value_type target) {
  return edge{value_type{}, std::move(target)};
}

template <typename Container>
edge<Container> edge<Container>::leaf(value_type source) {
  return edge{std::move(source), value_type{}};
}

}  // namespace vertex
