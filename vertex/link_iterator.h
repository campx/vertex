#pragma once

#include <iterator>
#include <type_traits>
#include <utility>

namespace vertex {

/** Follows links in Node by performing look-ups in a Container */
template <typename Container>
class link_iterator {
 public:
  using iterator_category = std::input_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using container_type = Container;
  using vertex_iterator = typename container_type::const_iterator;
  using vertex_type = typename container_type::value_type::second_type;
  using child_iterator = typename vertex_type::container_type::const_iterator;
  using value_type = typename Container::value_type;
  using self_type = link_iterator<Container>;
  using reference = const value_type&;
  using pointer = const value_type*;

  link_iterator();
  link_iterator(Container& vertices, const vertex_type* parent,
                child_iterator child);

  child_iterator position() const;
  self_type begin() const;
  self_type end() const;
  self_type& operator++();
  const self_type operator++(int dummy);
  reference operator*();
  pointer operator->();

  bool operator!=(const self_type& rhs) const;
  bool operator==(const self_type& rhs) const;

 private:
  Container* vertices_;
  const vertex_type* parent_;
  vertex_iterator child_;
  child_iterator chit_;
};

template <typename Container>
link_iterator<Container>::link_iterator() {}

template <typename Container>
link_iterator<Container>::link_iterator(Container& vertices,
                                        const vertex_type* parent,
                                      child_iterator child)
    : vertices_(&vertices),
      parent_(parent),
      child_(vertices_->cend()),
      chit_(std::move(child)) {}

template <typename Container>
typename link_iterator<Container>::child_iterator
link_iterator<Container>::position() const {
  return chit_;
}

template <typename Container>
typename link_iterator<Container>::self_type link_iterator<Container>::begin()
    const {
  return self_type(*vertices_, parent_, parent_->links().begin());
}

template <typename Container>
typename link_iterator<Container>::self_type link_iterator<Container>::end()
    const {
  return self_type(*vertices_, parent_, parent_->links().end());
}

template <typename Container>
typename link_iterator<Container>::self_type&
link_iterator<Container>::operator++() {
  ++chit_;
  return *this;
}

template <typename Container>
const typename link_iterator<Container>::self_type
link_iterator<Container>::operator++(int dummy) {
  (void)dummy;
  auto copy = *this;
  ++*this;
  return copy;
}

template <typename Container>
typename link_iterator<Container>::reference
    link_iterator<Container>::operator*() {
  child_ = vertices_->find(*chit_);
  return *child_;
}

template <typename Container>
typename link_iterator<Container>::pointer
    link_iterator<Container>::operator->() {
  return &(*this);
}

template <typename Container>
bool link_iterator<Container>::operator==(const self_type& rhs) const {
  return vertices_ == rhs.vertices_ && parent_ == rhs.parent_ &&
         chit_ == rhs.chit_;
}

template <typename Container>
bool link_iterator<Container>::operator!=(const self_type& rhs) const {
  return !(*this == rhs);
}

}  // namespace vertex
