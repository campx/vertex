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
    auto traversal = std::make_shared<Traversal>(vertices, vertex);
    return VertexIterator<Traversal>(traversal);
}

template <typename Traversal>
Iterator<Traversal, VertexAccessor<Traversal>>
VertexIterator(const typename std::shared_ptr<Traversal>& traversal)
{
    using Accessor = VertexAccessor<Traversal>;
    using vertex_type = typename Traversal::vertex_type;
    auto accessor = Accessor(
        [](const Traversal& t) -> const vertex_type& { return t.vertex(); });
    return make_iterator(traversal, accessor);
}

} // namespace vertex
