#pragma once
#include <memory>
#include <queue>

namespace vertex
{

/** Traversal predicate which always returns the given Value */
template <typename T, bool Value = true>
struct NullUnary
{
    using argument_type = T;
    using result_type = bool;

    result_type operator()(const argument_type& element)
    {
        return Value;
    }
};

/********************************************************************************/

/**@todo Create PredicatedIterator which skips items for which a predicate is
 * false
 *
 * Use ForwardIterator from objex to convert EdgeIterator to VertexIterator?
 * Or just create VertexIterator */

/**
 * @todo Write a breadth-first iterator, which takes a predicate for
 * traversal, by default a function which always returns true */

/** Iterates through all edges of a vertex, dereferencing to a edge-vertex
 * pair
 */
template <typename VertexStore>
class VertexEdgeIterator
    : public std::iterator<std::input_iterator_tag,
                           typename VertexStore::value_type>
{
public:
    using self_type = VertexEdgeIterator<VertexStore>;

    using edge_type = typename VertexStore::key_type;
    using mapped_type = typename VertexStore::mapped_type;
    using value_type = typename VertexStore::value_type;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::input_iterator_tag;

    using vertex_iterator = typename VertexStore::const_iterator;
    using edge_container = typename mapped_type::container_type;
    using edge_iterator = typename edge_container::const_iterator;

    VertexEdgeIterator(const VertexStore* vertices,
                       edge_iterator edge_it = edge_iterator{});
    VertexEdgeIterator(const VertexEdgeIterator&) = default;
    VertexEdgeIterator& operator=(const VertexEdgeIterator&) = default;

    vertex_iterator vertex();

    self_type operator++();
    self_type operator++(int dummy);
    self_type operator--();
    self_type operator--(int dummy);
    reference operator*();
    pointer operator->();

    bool operator==(const self_type& rhs) const;
    bool operator!=(const self_type& rhs) const;

private:
    const VertexStore* vertices_;
    edge_iterator edge_it_;
};

/************************************************************************************/

template <typename VertexStore>
VertexEdgeIterator<VertexStore>::VertexEdgeIterator(
    const VertexStore* vertices,
    typename VertexEdgeIterator<VertexStore>::edge_iterator edge_it)
    : vertices_(vertices), edge_it_(std::move(edge_it))
{
}

template <typename VertexStore>
typename VertexEdgeIterator<VertexStore>::vertex_iterator
VertexEdgeIterator<VertexStore>::vertex()
{
    // need to add an iterator to forest which will contain a pair of
    // edge,vertex
    // from find with the given edge
    auto result = vertices_->find(*(edge_it_));
    return result;
}

template <typename VertexStore>
typename VertexEdgeIterator<VertexStore>::self_type
    VertexEdgeIterator<VertexStore>::operator++()
{
    ++edge_it_;
    return *this;
}

template <typename VertexStore>
typename VertexEdgeIterator<VertexStore>::self_type
    VertexEdgeIterator<VertexStore>::operator++(int dummy)
{
    dummy++;
    auto tmp = *this;
    ++edge_it_;
    return tmp;
}

template <typename VertexStore>
typename VertexEdgeIterator<VertexStore>::self_type
    VertexEdgeIterator<VertexStore>::operator--()
{
    --edge_it_;
    return *this;
}

template <typename VertexStore>
typename VertexEdgeIterator<VertexStore>::self_type
    VertexEdgeIterator<VertexStore>::operator--(int dummy)
{
    dummy--;
    auto tmp = *this;
    --edge_it_;
    return tmp;
}

template <typename VertexStore>
typename VertexEdgeIterator<VertexStore>::reference
    VertexEdgeIterator<VertexStore>::operator*()
{
    return vertex().operator*();
}

template <typename VertexStore>
typename VertexEdgeIterator<VertexStore>::pointer
    VertexEdgeIterator<VertexStore>::operator->()
{
    return vertex().operator->();
}

template <typename VertexStore>
bool VertexEdgeIterator<VertexStore>::
operator==(const VertexEdgeIterator& rhs) const
{
    return vertices_ == rhs.vertices_ && edge_it_ == rhs.edge_it_;
}

template <typename VertexStore>
bool VertexEdgeIterator<VertexStore>::
operator!=(const VertexEdgeIterator& rhs) const
{
    return !(*this == rhs);
}

} // namespace vertex
