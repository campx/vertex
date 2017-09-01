#pragma once
#include <vertex/Iterator.h>

namespace vertex
{

template <typename Traversal>
using EdgeAccessor =
    std::function<const typename Traversal::edge_type&(const Traversal&)>;

template <typename Traversal>
Iterator<Traversal, EdgeAccessor<Traversal>>
EdgeIterator(typename Traversal::vertex_store_type* vertices,
             typename Traversal::vertex_store_type::iterator vertex)
{
    auto edge_accessor = std::mem_fn(&Traversal::edge);
    using iterator_type = Iterator<Traversal, EdgeAccessor<Traversal>>;
    return iterator_type(vertices, vertex, edge_accessor);
}

template <typename Traversal>
Iterator<Traversal, EdgeAccessor<Traversal>>
EdgeIterator(const typename std::shared_ptr<Traversal>& traversal)
{
    auto edge_accessor = std::mem_fn(&Traversal::edge);
    using iterator_type = Iterator<Traversal, EdgeAccessor<Traversal>>;
    return iterator_type(traversal, edge_accessor);
}

} // namespace vertex
