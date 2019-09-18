#pragma once
#include <vertex/Node.h>
#include <algorithm>
#include <cassert>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <type_traits>

namespace vertex {

/** ManagedContainer is a specialised AssociativeArray with reference
 * counting to ensure unreferenced Node objects are deleted from storage.
 * Container is a map which stores Node objects by Key
 * EdgeMap is a multimap which stores Node parents as pairs of Edge objects
 * Container MUST NOT invalidate iterators on insertion or deletion
 */
template <typename Container, typename EdgeMap>
class ManagedContainer {
 public:
  static_assert(std::is_same<typename Container::key_type,
                             typename EdgeMap::key_type>::value,
                "Container::key_type != EdgeMap::key_type");
  static_assert(std::is_same<typename EdgeMap::key_type,
                             typename EdgeMap::value_type::second_type>::value,
                "EdgeMap::key_type != EdgeMap::value_type::second_type");

  using key_type = typename Container::key_type;
  using mapped_type = typename Container::mapped_type;
  using value_type = typename Container::value_type;
  using size_type = typename Container::size_type;
  using difference_type = typename Container::difference_type;
  using key_compare = typename Container::key_compare;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = typename Container::pointer;
  using const_pointer = typename Container::const_pointer;
  using iterator = typename Container::iterator;
  using const_iterator = typename Container::const_iterator;
  using reverse_iterator = typename Container::reverse_iterator;
  using const_reverse_iterator = typename Container::const_reverse_iterator;

  explicit ManagedContainer(Container vertices = Container(),
                            EdgeMap edges = EdgeMap());

  iterator begin() const;
  const_iterator cbegin() const;
  iterator end() const;
  const_iterator cend() const;

  /** Insert a vertex, creating edges from all its children
   * Every child must exist */
  std::pair<iterator, bool> insert(const value_type& value);

  /** Find the vertex stored under the given key */
  iterator find(const key_type& key);

  /** Remove vertex at the specified position.
   *  The reference count of all child vertices will be decremented */
  iterator erase(iterator pos);

  /** Get reference count for vertex with given key */
  size_type count(const key_type& key);

  /** Erase the whole forest */
  void clear();

 private:
  using edge_type = typename EdgeMap::value_type;
  using edge_iterator = typename EdgeMap::iterator;

  /** Get an iterator to the edge matching the argument */
  edge_iterator find(const edge_type& edge);

  /** Remove the given vertex edge */
  edge_iterator erase(const edge_type& edge);

  /** Remove the vertex edge at the given position.
   * The given position must be valid. */
  edge_iterator erase(edge_iterator pos);

  Container vertices_;
  EdgeMap edges_;
};

template <typename V, typename E>
ManagedContainer<V, E>::ManagedContainer(V vertices, E edges)
    : vertices_(std::move(vertices)), edges_(std::move(edges)) {}

template <typename V, typename E>
std::pair<typename ManagedContainer<V, E>::iterator, bool>
ManagedContainer<V, E>::insert(
    const typename ManagedContainer<V, E>::value_type& value) {
  auto result = vertices_.insert(value); /** store the vertex */
  for (const auto& link :
       value.second
           .children()) {  // add a edge from vertex to each of its children
    edges_.insert(std::make_pair(link.key(), value.first));
  }
  return result;
}

template <typename V, typename E>
typename ManagedContainer<V, E>::iterator ManagedContainer<V, E>::find(
    const typename ManagedContainer<V, E>::key_type& key) {
  return vertices_.find(key);
}

template <typename V, typename E>
typename ManagedContainer<V, E>::iterator ManagedContainer<V, E>::erase(
    typename ManagedContainer<V, E>::iterator pos) {
  auto result = pos;
  const auto& edge = pos->first;
  const auto& vertex = pos->second;
  assert(vertex.self_edge() == edge);

  if (edges_.count(edge) == 0) {  // Erase vertex iff no references to it
    for (const auto& child :
         vertex.edges_) {  // remove edges from vertex to its children
      auto child_edge = edge_type(child, edge);
      erase(child_edge);
    }
    result = vertices_.erase(pos);
  }
  return result;
}

/** TODO rewrite using traversal */
template <typename V, typename E>
typename ManagedContainer<V, E>::edge_iterator ManagedContainer<V, E>::erase(
    typename ManagedContainer<V, E>::edge_iterator pos) {
  auto result = pos;
  std::queue<edge_iterator> to_visit;
  to_visit.push(pos);
  while (!to_visit.empty()) {
    pos = to_visit.front();
    to_visit.pop();
    assert(pos != edges_.end());
    auto child = pos->first;
    result = edges_.erase(pos);
    if (0 == edges_.count(
                 child)) {  // reference count of child vertex is zero, so erase
      auto vertex = vertices_.find(child);
      if (vertices_.end() != vertex) {
        for (const auto& grandchild : vertex->second.edges_) {
          auto it = find(edge_type(grandchild, vertex->first));
          to_visit.push(it);
        }
        vertices_.erase(vertex);
      }
    }
  }
  return result;
}

template <typename V, typename E>
typename ManagedContainer<V, E>::edge_iterator ManagedContainer<V, E>::find(
    const typename ManagedContainer<V, E>::edge_type& edge) {
  auto result = edges_.cend();
  auto range = edges_.equal_range(edge.first);
  auto it = std::find(range.first, range.second, edge);
  if (it != range.second) {
    result = it;
  }
  return result;
}

template <typename V, typename E>
typename ManagedContainer<V, E>::edge_iterator ManagedContainer<V, E>::erase(
    const typename ManagedContainer<V, E>::edge_type& edge) {
  auto result = find(edge);
  if (result != edges_.end()) {
    result = erase(result);
  }
  return result;
}

template <typename V, typename E>
typename ManagedContainer<V, E>::size_type ManagedContainer<V, E>::count(
    const typename ManagedContainer::key_type& key) {
  return edges_.count(key);
}

template <typename V, typename E>
void ManagedContainer<V, E>::clear() {
  edges_.clear();
  vertices_.clear();
}

}  // namespace vertex
