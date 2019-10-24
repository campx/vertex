#pragma once

#include <vertex/iterator_recorder.h>
#include <vertex/path.h>
#include <vertex/pre_order_traversal.h>
#include <algorithm>
#include <boost/iterator/transform_iterator.hpp>
#include <functional>
#include <vector>

namespace vertex {

template <typename Container>
std::pair<typename Container::iterator, bool> unique_insert(
    Container& container, typename Container::value_type&& value) {
  using result_type = std::pair<typename Container::iterator, bool>;
  auto it = std::find(container.begin(), container.end());
  return it == container.end() ? result_type{container.emplace(it, value), true}
                               : result_type{it, false};
}

/** PathMap traverses a graph, matching a Link sequence using a given
 * Compare function, returning an array of matching Link,Node pairs */
template <typename Container>
class path_map {
 public:
  using key_type = vertex::path<Container>;
  using mapped_type = typename Container::mapped_type;
  using value_type = std::pair<key_type, mapped_type>;

 private:
  class decoder {
   public:
    decoder() = default;
    explicit decoder(const Container& nodes);
    value_type operator()(const typename Container::value_type& value) const;

   private:
    mutable key_type path_;
    const Container* nodes_;
  };

 public:
  using size_type = typename Container::size_type;
  using difference_type = typename Container::difference_type;
  using allocator_type = typename Container::allocator_type;
  using pointer = typename Container::pointer;
  using const_pointer = typename Container::const_pointer;

  using traversal_type = pre_order_traversal<Container>;

  using transform_type = boost::transform_iterator<decoder, traversal_type,
                                                   value_type, value_type>;

  using iterator = toolbox::iterator_recorder<transform_type>;
  using const_iterator = iterator;

  explicit path_map(Container& nodes);

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
};

template <typename Container>
path_map<Container>::path_map(Container& nodes)
    : nodes_(&nodes), root_(nodes_->end()) {}

template <typename Container>
path_map<Container>::decoder::decoder(const Container& nodes)
    : nodes_(&nodes) {}

template <typename Container>
typename path_map<Container>::value_type
path_map<Container>::decoder::operator()(
    const typename Container::value_type& value) const {
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

template <typename Container>
const typename Container::const_iterator& path_map<Container>::root() const {
  return root_;
}

template <typename Container>
path_map<Container>& path_map<Container>::root(
    typename Container::const_iterator value) {
  root_ = value;
  return *this;
}

template <typename Container>
Container& path_map<Container>::nodes() const {
  return *nodes_;
}

template <typename Container>
typename path_map<Container>::iterator path_map<Container>::begin() const {
  auto first = traversal_type(nodes(), root_);
  auto last = first.end();
  first = first == last ? first : ++first;  // don't include root in results
  auto transformer = transform_type(first, decoder{nodes()});
  return iterator(transformer);
}

template <typename Container>
typename path_map<Container>::iterator path_map<Container>::cbegin() const {
  return begin();
}

template <typename Container>
typename path_map<Container>::iterator path_map<Container>::end() const {
  auto traversal = traversal_type(nodes(), root_).end();
  auto transformer = transform_type(traversal, decoder{nodes()});
  return iterator(transformer);
}

template <typename Container>
typename path_map<Container>::iterator path_map<Container>::cend() const {
  return end();
}

template <typename Container>
typename path_map<Container>::iterator path_map<Container>::search(
    const key_type& p) const {
  if (root_ == nodes().end()) {
    return end();
  }
  key_type path = key_type{root_->first};  // pre-pend root to search path
  path.insert(path.end(), p.begin(), p.end());
  auto predicate = [&path](const auto& e) -> bool {
    return e.is_root() || path.end() != std::search(path.begin(), path.end(),
                                                    e.begin(), e.end());
  };
  auto first = traversal_type(nodes(), root_, predicate);
  auto last = first.end();
  first = first == last ? first : ++first;  // don't include root in results
  auto first_transformer = transform_type(first, decoder{nodes()});
  auto last_transformer = transform_type(last, decoder{nodes()});
  auto begin = iterator(first_transformer);
  auto end = iterator(last_transformer);
  auto result = begin;
  for (auto it = begin; it != end; ++it) {
    result = it;
  }
  return result;
}

template <typename Container>
typename path_map<Container>::iterator path_map<Container>::find(
    const key_type& p) const {
  auto match = search(p);
  if (match != end() && match->first.size() != p.size()) {
    match = end();
  }
  return match;
}

template <typename Container>
std::pair<typename path_map<Container>::iterator, bool>
path_map<Container>::insert(const value_type& value) {
  auto result = std::make_pair(search(value.first), false);
  result.second = result.first == end() || value.first != result.first->first;
  if (!result.second) return result;  // return existing element at path
  auto prit = value.first.rbegin();   // parent reverse iterator
  auto chit = nodes().end();          // child iterator
  auto node = value.second;
  auto inserted = false;
  std::tie(chit, inserted) = insert_or_assign(*prit++, node);
  for (auto rend = value.first.rend(); prit != rend; ++prit) {
    // loop backwards through path, inserting parents where they don't exist
    if (result.first != end()) {                  // search results not empty
      if (result.first->first.back() == *prit) {  // match exists at this level
        node = result.first->second;  // Copy existing node for this link
        if (chit != nodes().end()) {  // add child link if missing
          if (node.links().end() == std::find(node.links().begin(),
                                              node.links().end(),
                                              chit->first)) {
            node.links().push_back(chit->first);
          }
        }
        --result.first;  // move up a level in search results
      }
    } else {                        // missing node at current level
      node = mapped_type();         // create empty node
      if (chit != nodes().end()) {  // add link from parent to child
        node.links().push_back(chit->first);
      }
    }  // upsert parent node and mark as child for next iteration
    std::tie(chit, inserted) = insert_or_assign(*prit, node);
  }
  node = mapped_type{};
  if (root_ != nodes().end()) {
    node = root_->second;
  }  // create or update root node to point to first element in path
  if (node.links().end() ==
      std::find(node.links().begin(), node.links().end(), chit->first)) {
    node.links().push_back(chit->first);
  }
  using container_key = typename Container::key_type;
  auto link = root_ == nodes().end() ? container_key{} : root_->first;
  std::tie(root_, inserted) = insert_or_assign(link, node);
  result.first = search(value.first);
  return result;
}

template <typename Container>
std::pair<typename Container::iterator, bool>
path_map<Container>::insert_or_assign(const typename Container::key_type& key,
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

template <typename Container>
typename path_map<Container>::iterator path_map<Container>::erase(iterator it) {
  const auto& path = it->first;
  const auto& key = path.back();
  nodes().erase(key);
  return search(it->first);
}

template <typename Container>
typename path_map<Container>::size_type path_map<Container>::erase(
    const path_map::key_type& p) {
  return nodes().erase(p.back());
}

}  // namespace vertex
