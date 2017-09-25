#pragma once
#include <memory>
#include <vertex/Iterator.h>

namespace vertex
{

/******************
 * A_________     *
 *  \  \  \  \    *
 *   \  \  \  \   *
 *    \  \  \  \  *
 *     B  C  D  E *
 *****************/
/** Array provides a std::vector like interface atop a tree of depth 1 */
template <typename VertexStore>
class Array
{
public:
    using vertex_iterator = typename VertexStore::iterator;
    using vertex_type = typename VertexStore::value_type;
    using vertex_container = typename VertexStore::vertex_container;
    using traversal_type = typename BredthFirstTraversal<VertexStore>;
    using iterator = Iterator<Bfs, VertexAccessor<Bfs>>;
    using const_iterator = const iterator;
    Array(VertexStore* vertices, typename VertexStore::iterator root);
    vertex_iterator root() const;
    VertexStore* vertices() const;

    void clear();
    iterator insert(const_iterator pos, const vertex_type& value);
    iterator erase(const_iterator pos);
    void push_back(const T& value);

    bool empty() const;
    iterator begin() const;
    iterator end() const;
    iterator insert(const vertex_type&);

private:
    vertex_iterator root_;
    VertexStore* vertices_;
};

template <typename VertexStore>
Array<VertexStore>::Array(VertexStore tree) : tree_(std::move(tree))
{
}

template <typename VertexStore>
void Array<VertexStore>::clear()
{
    auto root_key = root()->first;
    auto vertex = root()->second;
    vertex.clear();
    vertices_->insert(vertex);
}

template <typename VertexStore>
void Array<VertexStore>::root(
    typename Array<VertexStore>::vertex_iterator vertex)
{
    tree_.root(vertex);
}

template <typename VertexStore>
typename Array<VertexStore>::vertex_iterator Array<VertexStore>::root() const
{
    return tree_.root();
}

template <typename VertexStore>
bool Array<VertexStore>::empty() const
{
    return begin() == end();
}

template <typename VertexStore>
typename Array<VertexStore>::iterator Array<VertexStore>::begin() const
{
    return iterator(&tree_.vertices(), tree_.root()->second.edges().begin());
}

template <typename VertexStore>
typename Array<VertexStore>::iterator Array<VertexStore>::end() const
{
    return iterator(&tree_.vertices(), tree_.root()->second.edges().end());
}

template <typename VertexStore>
typename Array<VertexStore>::iterator Array<VertexStore>::insert(
    const typename Array<VertexStore>::const_iterator& pos,
    const typename Array<VertexStore>::vertex_type& value)
{
}

} // namespace vertex
