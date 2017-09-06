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
    auto traversal = std::make_shared<Traversal>(vertices, vertex);
    return EdgeIterator<Traversal>(traversal);
}

template <typename Traversal>
Iterator<Traversal, EdgeAccessor<Traversal>>
EdgeIterator(const typename std::shared_ptr<Traversal>& traversal)
{
    using Accessor = EdgeAccessor<Traversal>;
    using edge_type = typename Traversal::edge_type;
    auto accessor = Accessor(
        [](const Traversal& t) -> const edge_type& { return t.edge(); });
    return make_iterator(traversal, accessor);
}

} // namespace vertex
