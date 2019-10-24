#pragma once

#include <vertex/edge.h>
#include <vertex/predicate.h>
#include <functional>
#include <utility>

namespace vertex {

template <typename Container, typename Impl>
class traversal {
 public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using container_type = Container;
  using vertex_iterator = typename Container::const_iterator;
  using vertex_type = typename Container::mapped_type;
  using child_iterator = typename vertex_type::container_type::iterator;
  using key_type = typename Container::key_type;
  using edge_type = edge<Container>;
  using predicate_type = std::function<bool(const edge_type&)>;

  using value_type = typename Container::value_type;
  using self_type = traversal<Container, Impl>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

 protected:
  class unconditional_traversal {
   public:
    bool operator()(const edge<Container>&) { return true; }
  };

 public:
  traversal() = default;

  traversal(const Container& vertices, typename Container::const_iterator root,
            predicate_type predicate = unconditional_traversal());

  const Container& vertices() const;
  const vertex_iterator& root() const;
  const vertex_iterator& position() const;
  const predicate_type& predicate() const;
  bool is_traversable(const edge_type& value);

  Impl begin() const;
  Impl end() const;
  Impl& operator++();
  Impl operator++(int dummy);
  const_reference operator*() const;
  const_pointer operator->() const;

  bool operator!=(const self_type& rhs) const;
  bool operator==(const self_type& rhs) const;

 protected:
  void position(const vertex_iterator& value);

 private:
  const Container* vertices_;
  vertex_iterator root_;
  vertex_iterator position_;
  predicate_type predicate_;
};

template <typename Container, typename Impl>
traversal<Container, Impl>::traversal(const Container& vertices,
                                      typename Container::const_iterator root,
                                      predicate_type predicate)
    : vertices_(&vertices),
      root_(std::move(root)),
      position_(root_),
      predicate_(std::move(predicate)) {}

template <typename Container, typename Impl>
const Container& traversal<Container, Impl>::vertices() const {
  return *vertices_;
}

template <typename Container, typename Impl>
const typename traversal<Container, Impl>::vertex_iterator&
traversal<Container, Impl>::root() const {
  return root_;
}

template <typename Container, typename Impl>
const typename traversal<Container, Impl>::vertex_iterator&
traversal<Container, Impl>::position() const {
  return position_;
}

template <typename Container, typename Impl>
const typename traversal<Container, Impl>::predicate_type&
traversal<Container, Impl>::predicate() const {
  return predicate_;
}

template <typename Container, typename Impl>
void traversal<Container, Impl>::position(
    const typename traversal<Container, Impl>::vertex_iterator& value) {
  position_ = value;
}

template <typename Container, typename Impl>
bool traversal<Container, Impl>::is_traversable(const edge<Container>& value) {
  return predicate_(value);
}

template <typename Container, typename Impl>
Impl traversal<Container, Impl>::begin() const {
  return Impl(vertices(), root(), predicate());
}

template <typename Container, typename Impl>
Impl traversal<Container, Impl>::end() const {
  auto result = Impl(vertices(), root(), predicate());
  result.position(vertices().end());
  return result;
}

template <typename Container, typename Impl>
Impl& traversal<Container, Impl>::operator++() {
  auto pImpl = static_cast<Impl*>(this);
  if (!pImpl->next()) {
    position(vertices().end());
  }
  return *pImpl;
}

template <typename Container, typename Impl>
Impl traversal<Container, Impl>::operator++(int dummy) {
  (void)dummy;
  auto pImpl = static_cast<Impl*>(this);
  auto copy = *pImpl;
  ++*this;
  return copy;
}

template <typename Container, typename Impl>
typename traversal<Container, Impl>::const_reference
    traversal<Container, Impl>::operator*() const {
  return position().operator*();
}

template <typename Container, typename Impl>
typename traversal<Container, Impl>::const_pointer
    traversal<Container, Impl>::operator->() const {
  return position().operator->();
}

template <typename Container, typename Impl>
bool traversal<Container, Impl>::operator==(
    const traversal<Container, Impl>& rhs) const {
  return position() == rhs.position() && root() == rhs.root();
}

template <typename Container, typename Impl>
bool traversal<Container, Impl>::operator!=(
    const traversal<Container, Impl>& rhs) const {
  return !(*this == rhs);
}

}  // namespace vertex
