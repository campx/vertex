#pragma once
#include <utility>

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

/** Base class for edge iterators
 * Implementations are required to implement an increment method, which should
 * set the current edge and vertex and return a bool to indicate
 * success/failure */
template <typename Traversal>
class EdgeIterator
{
public:
    using vertex_store_type = typename Traversal::vertex_store_type;
    using vertex_iterator = typename vertex_store_type::iterator;

    EdgeIterator(vertex_store_type* vertices, vertex_iterator vertex);
    EdgeIterator(const EdgeIterator&) = default;
    EdgeIterator(EdgeIterator&&) = default;
    EdgeIterator& operator=(const EdgeIterator&) = default;
    EdgeIterator& operator=(EdgeIterator&&) = default;

    using self_type = EdgeIterator<Traversal>;
    using value_type = std::pair<typename vertex_store_type::key_type,
                                 typename vertex_store_type::key_type>;
    using reference = value_type&;
    using pointer = value_type*;

    self_type begin() const;
    self_type end() const;
    self_type& operator++();
    self_type& operator++(int dummy);
    reference operator*();
    pointer operator->();

    const value_type& edge() const;
    reference edge();
    vertex_iterator root() const;
    vertex_iterator vertex() const;
    vertex_store_type* vertices() const;
    void edge(const value_type& value);

    bool operator!=(const self_type& rhs) const;
    bool operator==(const self_type& rhs) const;

private:
    Traversal traverse_;
    value_type edge_;
};

template <typename Traversal>
EdgeIterator<Traversal>::EdgeIterator(
    vertex_store_type* vertices, typename vertex_store_type::iterator vertex)
    : traverse_(Traversal(vertices, std::move(vertex))),
      edge_(traverse_().first)
{
}

template <typename Traversal>
typename EdgeIterator<Traversal>::vertex_iterator
EdgeIterator<Traversal>::root() const
{
    return traverse_.root();
}

template <typename Traversal>
typename EdgeIterator<Traversal>::vertex_iterator
EdgeIterator<Traversal>::vertex() const
{
    return traverse_.vertex();
}

template <typename Traversal>
const typename EdgeIterator<Traversal>::value_type&
EdgeIterator<Traversal>::edge() const
{
    return edge_;
}

template <typename Traversal>
typename EdgeIterator<Traversal>::reference EdgeIterator<Traversal>::edge()
{
    return edge_;
}

template <typename Traversal>
void EdgeIterator<Traversal>::edge(
    const typename EdgeIterator<Traversal>::value_type& value)
{
    edge_ = value;
}

template <typename Traversal>
EdgeIterator<Traversal> EdgeIterator<Traversal>::begin() const
{
    return EdgeIterator<Traversal>(traverse_.vertices(), traverse_.root());
}

template <typename Traversal>
EdgeIterator<Traversal> EdgeIterator<Traversal>::end() const
{
    return EdgeIterator<Traversal>(vertices(), vertices()->end());
}

template <typename Traversal>
EdgeIterator<Traversal>& EdgeIterator<Traversal>::operator++()
{
    auto incremented = traverse_();
    if (!incremented.second)
    {
        *this = end();
    }
    else
    {
        edge(incremented.first);
    }
    return *this;
}

template <typename Traversal>
EdgeIterator<Traversal>& EdgeIterator<Traversal>::operator++(int dummy)
{
    (void)dummy;
    auto copy = *this;
    ++*this;
    return copy;
}

template <typename Traversal>
typename EdgeIterator<Traversal>::reference EdgeIterator<Traversal>::
operator*()
{
    return edge();
}

template <typename Traversal>
typename EdgeIterator<Traversal>::pointer EdgeIterator<Traversal>::operator->()
{
    return &edge();
}

template <typename Traversal>
bool EdgeIterator<Traversal>::
operator==(const EdgeIterator<Traversal>& rhs) const
{
    return edge() == rhs.edge() && vertex() == rhs.vertex() &&
           root() == rhs.root();
}

template <typename Traversal>
bool EdgeIterator<Traversal>::
operator!=(const EdgeIterator<Traversal>& rhs) const
{
    return !(*this == rhs);
}

template <typename Traversal>
typename EdgeIterator<Traversal>::vertex_store_type*
EdgeIterator<Traversal>::vertices() const
{
    return traverse_.vertices();
}

} // namespace vertex
