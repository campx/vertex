#pragma once
#include <algorithm>
#include <cassert>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <type_traits>
#include <vertex/Vertex.h>

namespace vertex
{

/** ManagedVertexStore is a specialised AssociativeArray with reference
 * counting to ensure unreferenced Vertex objects are deleted from storage.
 * VertexStore is a map which stores Vertex objects by Key
 * EdgeStore is a multimap which stores Vertex parents as pairs of Edge objects
 * VertexStore MUST NOT invalidate iterators on insertion or deletion
 */
template <typename VertexStore, typename EdgeStore>
class ManagedVertexStore
{
public:
    static_assert(std::is_same<typename VertexStore::key_type,
                               typename EdgeStore::key_type>::value,
                  "VertexStore::key_type != EdgeStore::key_type");
    static_assert(
        std::is_same<typename EdgeStore::key_type,
                     typename EdgeStore::value_type::second_type>::value,
        "EdgeStore::key_type != EdgeStore::value_type::second_type");

    using key_type = typename VertexStore::key_type;
    using mapped_type = typename VertexStore::mapped_type;
    using value_type = typename VertexStore::value_type;
    using size_type = typename VertexStore::size_type;
    using difference_type = typename VertexStore::difference_type;
    using key_compare = typename VertexStore::key_compare;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename VertexStore::pointer;
    using const_pointer = typename VertexStore::const_pointer;
    using iterator = typename VertexStore::iterator;
    using const_iterator = typename VertexStore::const_iterator;
    using reverse_iterator = typename VertexStore::reverse_iterator;
    using const_reverse_iterator =
        typename VertexStore::const_reverse_iterator;

    explicit ManagedVertexStore(VertexStore vertices = VertexStore(),
                                EdgeStore edges = EdgeStore());

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

    /** Erase the whole forest */
    void clear();

private:
    using edge_type = typename EdgeStore::value_type;
    using edge_iterator = typename EdgeStore::iterator;

    /** Get an iterator to the edge matching the argument */
    edge_iterator find(const edge_type& edge);

    /** Remove the given vertex edge */
    edge_iterator erase(const edge_type& edge);

    /** Remove the vertex edge at the given position.
     * The given position must be valid. */
    edge_iterator erase(edge_iterator pos);

    VertexStore vertices_;
    EdgeStore edges_;
};

template <typename V, typename E>
ManagedVertexStore<V, E>::ManagedVertexStore(V vertices, E edges)
    : vertices_(std::move(vertices)), edges_(std::move(edges))
{
}

template <typename V, typename E>
std::pair<typename ManagedVertexStore<V, E>::iterator, bool>
ManagedVertexStore<V, E>::insert(
    const typename ManagedVertexStore<V, E>::value_type& value)
{
    auto result = vertices_.insert(value); /** store the vertex */
    for (const auto& edge : value.second.children())
    { // add a edge from vertex to each of its children
        edges_.insert(std::make_pair(edge, value.first));
    }
    return result;
}

template <typename V, typename E>
typename ManagedVertexStore<V, E>::iterator ManagedVertexStore<V, E>::find(
    const typename ManagedVertexStore<V, E>::key_type& key)
{
    return vertices_.find(key);
}

template <typename V, typename E>
typename ManagedVertexStore<V, E>::iterator ManagedVertexStore<V, E>::erase(
    typename ManagedVertexStore<V, E>::iterator pos)
{
    auto result = pos;
    const auto& edge = pos->first;
    const auto& vertex = pos->second;
    assert(vertex.self_edge() == edge);

    if (edges_.count(edge) == 0)
    { // Erase vertex iff no references to it
        for (const auto& child : vertex.edges_)
        { // remove edges from vertex to its children
            auto child_edge = edge_type(child, edge);
            erase(child_edge);
        }
        result = vertices_.erase(pos);
    }
    return result;
}

template <typename V, typename E>
typename ManagedVertexStore<V, E>::edge_iterator
ManagedVertexStore<V, E>::erase(
    typename ManagedVertexStore<V, E>::edge_iterator pos)
{
    auto result = pos;
    std::queue<edge_iterator> to_visit;
    to_visit.push(pos);
    while (!to_visit.empty())
    {
        pos = to_visit.front();
        to_visit.pop();
        assert(pos != edges_.end());
        auto child = pos->first;
        result = edges_.erase(pos);
        if (0 == edges_.count(child))
        { // reference count of child vertex is zero, so erase
            auto vertex = vertices_.find(child);
            if (vertices_.end() != vertex)
            {
                for (const auto& grandchild : vertex->second.edges_)
                {
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
typename ManagedVertexStore<V, E>::edge_iterator
ManagedVertexStore<V, E>::find(
    const typename ManagedVertexStore<V, E>::edge_type& edge)
{
    auto result = edges_.cend();
    auto range = edges_.equal_range(edge.first);
    auto it = std::find(range.first, range.second, edge);
    if (it != range.second)
    {
        result = it;
    }
    return result;
}

template <typename V, typename E>
typename ManagedVertexStore<V, E>::edge_iterator
ManagedVertexStore<V, E>::erase(
    const typename ManagedVertexStore<V, E>::edge_type& edge)
{
    auto result = find(edge);
    if (result != edges_.end())
    {
        result = erase(result);
    }
    return result;
}

template <typename V, typename E>
void ManagedVertexStore<V, E>::clear()
{
    edges_.clear();
    vertices_.clear();
}

} // namespace vertex
