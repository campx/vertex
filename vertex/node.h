#pragma once

#include <algorithm>
#include <vector>

namespace vertex {

/** A node in a tree, which has data of type T, and a collection of Link
 * pointers to child nodes.
 * Provides a set-like interface for adding and removing children
 *
 * Impl is required to implement get_links() and get_element(), from which
 * we offer comparison operators, swap, and a std::set-like interface
 * */
template <typename Impl, typename Link, typename T,
          typename Container = std::vector<Link>>
class node {
 public:
  using container_type = Container;
  using value_type = typename Container::value_type;
  using iterator = typename Container::iterator;
  using const_iterator = typename Container::const_iterator;
  using element_type = std::remove_reference_t<T>;
  using key_type = std::remove_reference_t<Link>;
  using size_type = std::size_t;
  using link_type = value_type;
  using pointer = T*;

  static_assert(std::is_same<key_type, value_type>::value);

  /** Exchanges the contents of the Node with those of other */
  void swap(node& other);

  /** Returns a pointer to the node data */
  pointer get() const;

  /** Returns the node data object */
  typename std::add_lvalue_reference<T>::type operator*();

  /** Returns the node data object */
  typename std::add_lvalue_reference<T>::type operator*() const;

  /** Dereferences pointer to the node data */
  pointer operator->() const;

  /** Returns an iterator to the beginning */
  const_iterator begin() const;

  /** Returns an iterator to the end */
  const_iterator end() const;

  /** Returns the number of links equivalent to the argument */
  template <typename K>
  size_type count(const K& x);

  /** Returns the number of links */
  [[nodiscard]] size_type size() const;

  /** Returns true if the node has no links */
  [[nodiscard]] bool empty() const;

  /** Inserts a link */
  std::pair<iterator, bool> insert(const value_type& link);

  /** Inserts links from the range [first, last) */
  template <typename InputIt>
  void insert(InputIt first, InputIt last);

  /** Erases a link */
  size_type erase(const key_type& key);

  /** Removes all links from the node */
  void clear() noexcept;

  /** Finds a link that compares equivalent to the value x */
  template <typename K>
  const_iterator find(const K& x) const;

  /** Compare the contents and children of a Node against another */
  bool operator==(const node& rhs) const;

  /** Compare the contents and children of a Node against another */
  bool operator!=(const node& rhs) const;

 private:
  const container_type& links() const;
  container_type& links();
};

template <typename Impl, typename Link, typename T, typename Container>
void node<Impl, Link, T, Container>::swap(node& other) {
  std::swap(this->link(), other.link());
  std::swap(this->value(), other.value());
}

template <typename Impl, typename Link, typename T, typename Container>
typename node<Impl, Link, T, Container>::pointer
node<Impl, Link, T, Container>::get() const {
  return static_cast<const Impl*>(this)->get_element();
}

template <typename Impl, typename Link, typename T, typename Container>
typename std::add_lvalue_reference<T>::type
    node<Impl, Link, T, Container>::operator*() {
  auto pimpl = static_cast<Impl*>(this);
  return pimpl->get_element();
}

template <typename Impl, typename Link, typename T, typename Container>
typename std::add_lvalue_reference<T>::type
    node<Impl, Link, T, Container>::operator*() const {
  auto pimpl = static_cast<const Impl*>(this);
  return const_cast<Impl*>(pimpl)->get_element();
}

template <typename Impl, typename Link, typename T, typename Container>
typename node<Impl, Link, T, Container>::pointer
    node<Impl, Link, T, Container>::operator->() const {
  return static_cast<const Impl*>(this)->get_element();
}

template <typename Impl, typename Link, typename T, typename Container>
typename node<Impl, Link, T, Container>::const_iterator
node<Impl, Link, T, Container>::begin() const {
  return links().begin();
}

template <typename Impl, typename Link, typename T, typename Container>
typename node<Impl, Link, T, Container>::const_iterator
node<Impl, Link, T, Container>::end() const {
  return links().end();
}

template <typename Impl, typename Link, typename T, typename Container>
const typename node<Impl, Link, T, Container>::container_type&
node<Impl, Link, T, Container>::links() const {
  auto pimpl = static_cast<const Impl*>(this);
  const auto& result = const_cast<Impl*>(pimpl)->get_links();
  return result;
}

template <typename Impl, typename Link, typename T, typename Container>
typename node<Impl, Link, T, Container>::container_type&
node<Impl, Link, T, Container>::links() {
  return static_cast<Impl*>(this)->get_links();
}

template <typename Impl, typename Link, typename T, typename Container>
template <typename InputIt>
void node<Impl, Link, T, Container>::insert(InputIt first, InputIt last) {
  for (auto it = first; it != last; ++it) {
    insert(*it);
  }
}

template <typename Impl, typename Link, typename T, typename Container>
template <typename K>
typename node<Impl, Link, T, Container>::size_type
node<Impl, Link, T, Container>::count(const K& x) {
  return std::count(begin(), end(), x);
}

template <typename Impl, typename Link, typename T, typename Container>
bool node<Impl, Link, T, Container>::operator==(
    const node<Impl, Link, T, Container>& rhs) const {
  return **this == *rhs && this->links() == rhs.links();
}

template <typename Impl, typename Link, typename T, typename Container>
bool node<Impl, Link, T, Container>::operator!=(
    const node<Impl, Link, T, Container>& rhs) const {
  return !(*this == rhs);
}

template <typename Impl, typename Link, typename T, typename Container>
std::pair<typename node<Impl, Link, T, Container>::iterator, bool>
node<Impl, Link, T, Container>::insert(const value_type& link) {
  auto first = links().begin();
  auto last = links().end();
  auto result = std::pair(std::find(first, last, link), true);
  result.second = result.first == last;
  if (result.second) {
    result.first = links().insert(result.first, link);
  }
  return result;
}

template <typename Impl, typename Link, typename T, typename Container>
typename node<Impl, Link, T, Container>::size_type
node<Impl, Link, T, Container>::size() const {
  return links().size();
}

template <typename Impl, typename Link, typename T, typename Container>
bool node<Impl, Link, T, Container>::empty() const {
  return links().empty();
}

template <typename Impl, typename Link, typename T, typename Container>
void node<Impl, Link, T, Container>::clear() noexcept {
  links().clear();
}

template <typename Impl, typename Link, typename T, typename Container>
typename node<Impl, Link, T, Container>::size_type
node<Impl, Link, T, Container>::erase(
    const typename node<Impl, Link, T, Container>::key_type& key) {
  auto it = begin();
  size_type result = 0;
  while (it != end()) {
    it = find(key);
    if (it != end()) {
      links().erase(it);
      ++result;
    }
  }
  return result;
}

template <typename Impl, typename Link, typename T, typename Container>

template <typename K>
typename node<Impl, Link, T, Container>::const_iterator
node<Impl, Link, T, Container>::find(const K& x) const {
  return std::find(begin(), end(), x);
}

}  // namespace vertex
