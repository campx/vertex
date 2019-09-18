#pragma once
#include <vertex/predicate.h>
#include <utility>

namespace vertex {

template <typename Container, typename Impl,
          typename Predicate = NullaryPredicate<Container, true>>
class traversal : public std::iterator<std::forward_iterator_tag,
                                       typename Container::mapped_type> {
 public:
  using container_type = Container;
  using vertex_iterator = typename Container::const_iterator;
  using vertex_type = typename Container::mapped_type;
  using child_iterator = typename vertex_type::container_type::iterator;
  using key_type = typename Container::key_type;
  using edge_type = std::pair<key_type, key_type>;
  using predicate_type = Predicate;

  using value_type = typename Container::value_type;
  using self_type = traversal<Container, Impl, Predicate>;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  traversal() = default;

  traversal(const Container& vertices, typename Container::const_iterator root,
            Predicate predicate = Predicate{});

  const Container& vertices() const;
  const vertex_iterator& root() const;
  const vertex_iterator& position() const;
  const predicate_type& predicate() const;
  bool isTraversible(const edge_type& value);

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
  void vertex(const vertex_type& value);
  void edge(const edge_type& value);

 private:
  const Container* vertices_;
  vertex_iterator root_;
  vertex_iterator position_;
  Predicate predicate_;
};

template <typename Container, typename Impl, typename Predicate>
traversal<Container, Impl, Predicate>::traversal(
    const Container& vertices, typename Container::const_iterator root,
    Predicate predicate)
    : vertices_(&vertices),
      root_(std::move(root)),
      position_(root_),
      predicate_(std::move(predicate)) {}

template <typename Container, typename Impl, typename Predicate>
const Container& traversal<Container, Impl, Predicate>::vertices() const {
  return *vertices_;
}

template <typename Container, typename Impl, typename Predicate>
const typename traversal<Container, Impl, Predicate>::vertex_iterator&
traversal<Container, Impl, Predicate>::root() const {
  return root_;
}

template <typename Container, typename Impl, typename Predicate>
const typename traversal<Container, Impl, Predicate>::vertex_iterator&
traversal<Container, Impl, Predicate>::position() const {
  return position_;
}

template <typename Container, typename Impl, typename Predicate>
const typename traversal<Container, Impl, Predicate>::predicate_type&
traversal<Container, Impl, Predicate>::predicate() const {
  return predicate_;
}

template <typename Container, typename Impl, typename Predicate>
void traversal<Container, Impl, Predicate>::position(
    const typename traversal<Container, Impl, Predicate>::vertex_iterator&
        value) {
  position_ = value;
}

template <typename Container, typename Impl, typename Predicate>
bool traversal<Container, Impl, Predicate>::isTraversible(
    const typename traversal<Container, Impl, Predicate>::edge_type& value) {
  return predicate_(value);
}

template <typename Container, typename Impl, typename Predicate>
Impl traversal<Container, Impl, Predicate>::begin() const {
  return Impl(vertices(), root(), predicate());
}

template <typename Container, typename Impl, typename Predicate>
Impl traversal<Container, Impl, Predicate>::end() const {
  auto result = Impl(vertices(), root(), predicate());
  result.position(vertices().end());
  return result;
}

template <typename Container, typename Impl, typename Predicate>
Impl& traversal<Container, Impl, Predicate>::operator++() {
  auto pImpl = static_cast<Impl*>(this);
  if (!pImpl->next()) {
    position(vertices().end());
  }
  return *pImpl;
}

template <typename Container, typename Impl, typename Predicate>
Impl traversal<Container, Impl, Predicate>::operator++(int dummy) {
  (void)dummy;
  auto pImpl = static_cast<Impl*>(this);
  auto copy = *pImpl;
  ++*this;
  return copy;
}

template <typename Container, typename Impl, typename Predicate>
typename traversal<Container, Impl, Predicate>::const_reference
    traversal<Container, Impl, Predicate>::operator*() const {
  return position().operator*();
}

template <typename Container, typename Impl, typename Predicate>
typename traversal<Container, Impl, Predicate>::const_pointer
    traversal<Container, Impl, Predicate>::operator->() const {
  return position().operator->();
}

template <typename Container, typename Impl, typename Predicate>
bool traversal<Container, Impl, Predicate>::operator==(
    const traversal<Container, Impl, Predicate>& rhs) const {
  return position() == rhs.position() && root() == rhs.root();
}

template <typename Container, typename Impl, typename Predicate>
bool traversal<Container, Impl, Predicate>::operator!=(
    const traversal<Container, Impl, Predicate>& rhs) const {
  return !(*this == rhs);
}

}  // namespace vertex
