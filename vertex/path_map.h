#pragma once

#include <vertex/iterator.h>
#include <vertex/iterator_recorder.h>
#include <vertex/iterator_transformer.h>
#include <vertex/pre_order_traversal.h>
#include <vertex/sequence_predicate.h>
#include <algorithm>
#include <functional>
#include <vector>

namespace vertex {

/** PathMap traverses a graph, matching a Link sequence using a given
 * Compare function, returning an array of matching Link,Node pairs */
template <typename Container,
          typename Compare = std::less<typename Container::key_type>>
class path_map {
 public:
  using key_type = std::vector<typename Container::key_type>;
  using mapped_type = typename Container::mapped_type;
  using value_type = std::pair<key_type, mapped_type>;

 private:
  class Decoder {
   public:
    Decoder() = default;

    explicit Decoder(Container& nodes);

    value_type operator()(const typename Container::value_type& value);

   private:
    key_type path_;
    Container* nodes_;
  };

 public:
  using size_type = typename Container::size_type;
  using difference_type = typename Container::difference_type;
  using allocator_type = typename Container::allocator_type;
  using pointer = typename Container::pointer;
  using const_pointer = typename Container::const_pointer;

  using Predicate =
      toolbox::sequence_predicate<typename key_type::const_iterator, Compare>;
  using Traversal = pre_order_traversal<Container, Predicate>;
  using Transformer = toolbox::iterator_transformer<Decoder, Traversal>;
  using iterator = toolbox::iterator_recorder<Transformer>;
  using const_iterator = iterator;

  explicit path_map(Container& nodes, Compare compare = Compare());

  const typename Container::const_iterator& root() const;

  path_map& root(typename Container::const_iterator value);

  Container& nodes() const;

  iterator begin() const;

  const_iterator cbegin() const;

  const_iterator cend() const;

  iterator end() const;

  /** Search for a path with partial matching
   * @return Value containing all matched path segments */
  iterator search(const key_type& p) const;

  /** Find a full path
   * @return Value of match or end() if the case of an incomplete match */
  iterator find(const key_type& p) const;

  /** Erase element pointed to by the given iterator */
  iterator erase(iterator it);

  /** Erase element at given path */
  size_type erase(const key_type& p);

  std::pair<iterator, bool> insert(const value_type& value);

 private:
  std::pair<typename Container::iterator, bool> insert_or_assign(
      const typename Container::key_type& key,
      typename Container::mapped_type& value);

  Container* nodes_;
  typename Container::const_iterator root_;
  Compare compare_;
};

template <typename Container, typename Compare>
path_map<Container, Compare>::path_map(Container& nodes, Compare compare)
    : nodes_(&nodes), root_(nodes_->end()), compare_(std::move(compare)) {}

template <typename Container, typename Compare>
path_map<Container, Compare>::Decoder::Decoder(Container& nodes)
    : nodes_(&nodes) {}

template <typename Container, typename Compare>
typename path_map<Container, Compare>::value_type
path_map<Container, Compare>::Decoder::operator()(
    const typename Container::value_type& value) {
  if (!path_.empty()) {
    auto it = path_.rbegin();
    for (auto end = path_.rend(); it != end; ++it) {
      auto node = nodes_->find(*it);
      if (node != nodes_->end()) {
        auto child_it = std::find(node->second.links().begin(),
                                  node->second.links().end(), value.first);
        if (child_it != node->second.links().end()) {
          break;  // this node contains the link
        }
      }
    }
    path_.erase(it.base(), path_.end());
  }
  path_.push_back(value.first);
  return std::make_pair(path_, value.second);
}

template <typename Container, typename Compare>
const typename Container::const_iterator& path_map<Container, Compare>::root()
    const {
  return root_;
}

template <typename Container, typename Compare>
path_map<Container, Compare>& path_map<Container, Compare>::root(
    typename Container::const_iterator value) {
  root_ = value;
  return *this;
}

template <typename Container, typename Compare>
Container& path_map<Container, Compare>::nodes() const {
  return *nodes_;
}

template <typename Container, typename Compare>
typename path_map<Container, Compare>::iterator
path_map<Container, Compare>::begin() const {
  auto first = Traversal(nodes(), root_);
  auto last = first.end();
  first = first == last ? first : ++first;  // don't include root in results
  auto decoder = Decoder(nodes());
  auto transformer = Transformer(first, decoder);
  return iterator(transformer);
}

template <typename Container, typename Compare>
typename path_map<Container, Compare>::iterator
path_map<Container, Compare>::cbegin() const {
  return begin();
}

template <typename Container, typename Compare>
typename path_map<Container, Compare>::iterator
path_map<Container, Compare>::end() const {
  auto traversal = Traversal(nodes(), root_).end();
  auto decoder = Decoder(nodes());
  auto transformer = Transformer(traversal, decoder);
  return iterator(transformer);
}

template <typename Container, typename Compare>
typename path_map<Container, Compare>::iterator
path_map<Container, Compare>::cend() const {
  return end();
}

template <typename Container, typename Compare>
typename path_map<Container, Compare>::iterator
path_map<Container, Compare>::search(const key_type& p) const {
  if (root_ == nodes().end()) {
    return end();
  }
  key_type path = key_type{root_->first};  // pre-pend root to search path
  path.insert(path.end(), p.begin(), p.end());
  auto predicate = Predicate(path.begin(), path.end(), compare_);
  auto first = Traversal(nodes(), root_, predicate);
  auto decoder = Decoder(nodes());
  auto last = first.end();
  first = first == last ? first : ++first;  // don't include root in results
  auto first_transformer = Transformer(first, decoder);
  auto last_transformer = Transformer(last, decoder);
  auto begin = iterator(first_transformer);
  auto end = iterator(last_transformer);
  auto result = toolbox::back(begin, end);
  return result;
}

template <typename Container, typename Compare>
typename path_map<Container, Compare>::iterator
path_map<Container, Compare>::find(const key_type& p) const {
  auto match = search(p);
  if (match != end() && match->first.size() != p.size()) {
    match = end();
  }
  return match;
}

template <typename Container, typename Compare>
std::pair<typename path_map<Container, Compare>::iterator, bool>
path_map<Container, Compare>::insert(const value_type& value) {
  auto result = std::make_pair(search(value.first), false);
  if (result.first != end() &&
      value.first.size() == result.first->first.size() &&
      std::equal(value.first.cbegin(), value.first.cend(),
                 result.first->first.cbegin())) {  // already exists
    return result;
  }
  result.second = true;
  auto path_it = value.first.rbegin();
  auto child_it = nodes().end();
  auto node = value.second;
  auto inserted = false;
  std::tie(child_it, inserted) = insert_or_assign(*path_it++, node);
  result.second = inserted;
  for (auto rend = value.first.rend(); path_it != rend; ++path_it) {
    if (result.first != end()) {
      if (!(compare_(result.first->first.back(), *path_it)) &&
          !(compare_(*path_it,
                     result.first->first
                         .back()))) {  // Copy existing node for this link
        node = result.first->second;
        if (child_it != nodes().end()) {
          if (node.links().end() == std::find(node.links().begin(),
                                              node.links().end(),
                                              child_it->first)) {
            node.links().push_back(child_it->first);
          }
        }
        --result.first;
      }
    } else {
      node = mapped_type();             // create empty node
      if (child_it != nodes().end()) {  // add link from parent to child
        node.links().push_back(child_it->first);
      }
    }
    std::tie(child_it, inserted) = insert_or_assign(*path_it, node);
  }
  node = mapped_type{};
  if (root_ != nodes().end()) {
    node = root_->second;
  }
  if (node.links().end() ==
      std::find(node.links().begin(), node.links().end(), child_it->first)) {
    node.links().push_back(child_it->first);
  }
  using container_key = typename Container::key_type;
  auto link = root_ == nodes().end() ? container_key{} : root_->first;
  std::tie(root_, inserted) = insert_or_assign(link, node);
  result.first = search(value.first);
  return result;
}

template <typename Container, typename Compare>
std::pair<typename Container::iterator, bool>
path_map<Container, Compare>::insert_or_assign(
    const typename Container::key_type& key,
    typename Container::mapped_type& value) {
  auto result = std::make_pair(nodes().find(key), true);
  if (result.first == nodes().end()) {
    result = nodes().emplace(std::make_pair(key, value));
  } else {
    result.first->second = value;
    result.second = false;
  }
  return result;
}

template <typename Container, typename Compare>
typename path_map<Container, Compare>::iterator
path_map<Container, Compare>::erase(iterator it) {
  const auto& path = it->first;
  const auto& key = path.back();
  nodes().erase(key);
  return search(it->first);
}

template <typename Container, typename Compare>
typename path_map<Container, Compare>::size_type
path_map<Container, Compare>::erase(const path_map::key_type& p) {
  return nodes().erase(p.back());
}

}  // namespace vertex
