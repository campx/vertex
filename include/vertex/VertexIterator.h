#pragma once
#include <vertex/Iterator.h>

namespace vertex
{

template <typename Traversal>
using VertexAccessor =
    std::function<const typename Traversal::vertex_type&(const Traversal&)>;

template <typename Traversal>
Iterator<Traversal, VertexAccessor<Traversal>>
VertexIterator(typename Traversal::vertex_store_type* vertices,
               typename Traversal::vertex_store_type::iterator vertex)
{
    auto vertex_accessor = std::mem_fn(&Traversal::vertex);
    using iterator_type = Iterator<Traversal, VertexAccessor<Traversal>>;
    return iterator_type(vertices, vertex, vertex_accessor);
}

template <typename Traversal>
Iterator<Traversal, VertexAccessor<Traversal>>
VertexIterator(const typename std::shared_ptr<Traversal>& traversal)
{
    auto vertex_accessor = std::mem_fn(&Traversal::vertex);
    using iterator_type = Iterator<Traversal, VertexAccessor<Traversal>>;
    return iterator_type(traversal, vertex_accessor);
}

} // namespace vertex
