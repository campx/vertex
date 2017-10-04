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
template <typename VertexMap>
class Array
{
public:
    using vertex_iterator = typename VertexMap::iterator;
    using vertex_type = typename VertexMap::value_type;
    using vertex_container = typename VertexMap::vertex_container;
    using traversal_type = typename BredthFirstTraversal<VertexMap>;
    using iterator = Iterator<Bfs, VertexAccessor<Bfs>>;
    using const_iterator = const iterator;
    Array(VertexMap* vertices, typename VertexMap::iterator root);
    vertex_iterator root() const;
    VertexMap* vertices() const;

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
    VertexMap* vertices_;
};

template <typename VertexMap>
Array<VertexMap>::Array(VertexMap tree) : tree_(std::move(tree))
{
}

template <typename VertexMap>
void Array<VertexMap>::clear()
{
    auto root_key = root()->first;
    auto vertex = root()->second;
    vertex.clear();
    vertices_->insert(vertex);
}

template <typename VertexMap>
void Array<VertexMap>::root(
    typename Array<VertexMap>::vertex_iterator vertex)
{
    tree_.root(vertex);
}

template <typename VertexMap>
typename Array<VertexMap>::vertex_iterator Array<VertexMap>::root() const
{
    return tree_.root();
}

template <typename VertexMap>
bool Array<VertexMap>::empty() const
{
    return begin() == end();
}

template <typename VertexMap>
typename Array<VertexMap>::iterator Array<VertexMap>::begin() const
{
    return iterator(&tree_.vertices(), tree_.root()->second.edges().begin());
}

template <typename VertexMap>
typename Array<VertexMap>::iterator Array<VertexMap>::end() const
{
    return iterator(&tree_.vertices(), tree_.root()->second.edges().end());
}

template <typename VertexMap>
typename Array<VertexMap>::iterator Array<VertexMap>::insert(
    const typename Array<VertexMap>::const_iterator& pos,
    const typename Array<VertexMap>::vertex_type& value)
{
}

} // namespace vertex
