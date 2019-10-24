#pragma once

#include <algorithm>
#include <vector>

namespace vertex {

/** A node in a tree, which has data of type T, and a collection of Link
 * pointers to child nodes.
 * Provides a set-like interface for adding and removing children
 * TODO make this CRTP, don't assume vector */
template <typename Link, typename T>
class node {
 public:
  using link_type = Link;
  using container_type = std::vector<Link>;
  using value_type = T;

  /** Create a Node with the given data value */
  explicit node(value_type data = value_type(),
                container_type links = container_type());

  node(const node&) = default;
  node(node&&) noexcept;
  node& operator=(const node&) = default;
  node& operator=(node&&) noexcept;

  /** Exchanges the contents of the Node with those of other */
  void swap(node& other);

  /** Get the out-bound links from this node */
  [[nodiscard]] const container_type& links() const;

  /** Set the out-bound links for this node */
  void links(const container_type& value);

  /** Get the data associated with this node */
  const value_type& data() const;

  /** Set the data associated with this node */
  void data(const value_type& value);

  /** Provide read-write access to the data */
  value_type& data();

  /** Provide read-write access to the links */
  container_type& links();

  /** Compare the contents and children of a Node against another */
  bool operator==(const node& rhs) const;

  /** Compare the contents and children of a Node against another */
  bool operator!=(const node& rhs) const;

 private:
  value_type data_;
  container_type links_;
};

template <typename Link, typename T>
node<Link, T>::node(value_type data, container_type links)
    : data_(std::move(data)), links_(std::move(links)) {}

template <typename Link, typename T>
void node<Link, T>::swap(node& other) {
  std::swap(links_, other.links_);
  std::swap(data_, other.data_);
}

template <typename Link, typename T>
const typename node<Link, T>::container_type& node<Link, T>::links() const {
  return links_;
}

template <typename Link, typename T>
void node<Link, T>::links(const typename node<Link, T>::container_type& value) {
  links_ = value;
}

template <typename Link, typename T>
const typename node<Link, T>::value_type& node<Link, T>::data() const {
  return data_;
}

template <typename Link, typename T>
void node<Link, T>::data(const typename node<Link, T>::value_type& value) {
  data_ = value;
}

template <typename Link, typename T>
typename node<Link, T>::value_type& node<Link, T>::data() {
  return data_;
}

template <typename Link, typename T>
typename node<Link, T>::container_type& node<Link, T>::links() {
  return links_;
}

template <typename Link, typename T>
bool node<Link, T>::operator==(const node<Link, T>& rhs) const {
  return data_ == rhs.data_ && links_ == rhs.links_;
}

template <typename Link, typename T>
bool node<Link, T>::operator!=(const node<Link, T>& rhs) const {
  return !(*this == rhs);
}

template <typename Link, typename T>
node<Link, T>::node(node&& other) noexcept
    : data_(std::move(other.data_)), links_(std::move(other.links_)) {}

template <typename Link, typename T>
node<Link, T>& node<Link, T>::operator=(node&& rhs) noexcept {
  data_ = std::move(rhs.data_);
  links_ = std::move(rhs.links_);
  return *this;
}

}  // namespace vertex
