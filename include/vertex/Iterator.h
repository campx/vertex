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

/** An Edge Iterator which traverses a VertexStore in bredth-first order */
template <typename VertexStore>
class BredthFirstTraversal
{
public:
    BredthFirstTraversal(VertexStore* vertices,
                         typename VertexStore::iterator vertex);
    BredthFirstTraversal(const BredthFirstTraversal&) = default;
    BredthFirstTraversal(BredthFirstTraversal&&) = default;
    BredthFirstTraversal& operator=(const BredthFirstTraversal&) = default;
    BredthFirstTraversal& operator=(BredthFirstTraversal&&) = default;

    using self_type = BredthFirstTraversal<VertexStore>;
    using value_type = typename VertexStore::mapped_type;
    using reference = value_type&;
    using pointer = value_type*;

    self_type begin() const;
    self_type end() const;
    self_type operator++();
    self_type operator++(int dummy);
    reference operator*();
    pointer operator->();

    bool operator!=(const self_type& rhs) const;
    bool operator==(const self_type& rhs) const;

private:
    void enqueue_children();
    using vertex_iterator = typename VertexStore::iterator;
    using vertex_type = typename VertexStore::mapped_type;
    using child_iterator = typename vertex_type::iterator;
    using key_type = typename VertexStore::key_type;
    std::queue<std::pair<key_type, key_type>> to_visit_;
    VertexStore* vertices_;
    vertex_iterator root_;
    vertex_iterator vertex_;
};

/************************************************************************************/

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
BredthFirstTraversal<VertexStore>::BredthFirstTraversal(
    VertexStore* vertices, typename VertexStore::iterator vertex)
    : vertices_(vertices), root_(std::move(vertex)), vertex_(root_)
{
    if (!vertices_->empty() && vertex_ != vertices_->end())
    {
        enqueue_children();
    }
}

template <typename VertexStore>
void BredthFirstTraversal<VertexStore>::enqueue_children()
{
    for (const auto& child : vertex_->second)
    {
        to_visit_.push(std::make_pair(vertex_->first, child));
    }
}

template <typename VertexStore>
typename BredthFirstTraversal<VertexStore>::self_type
BredthFirstTraversal<VertexStore>::begin() const
{
    return BredthFirstTraversal(vertices_, root_);
}

template <typename VertexStore>
typename BredthFirstTraversal<VertexStore>::self_type
BredthFirstTraversal<VertexStore>::end() const
{
    return BredthFirstTraversal(vertices_, vertices_->end());
}

template <typename VertexStore>
typename BredthFirstTraversal<VertexStore>::self_type
    BredthFirstTraversal<VertexStore>::operator++()
{
    if (to_visit_.empty())
    {
        *this = end();
    }
    else
    {
        auto edge = to_visit_.front();
        to_visit_.pop();
        vertex_ = vertices_->find(edge.second);
        enqueue_children();
    }
    return *this;
}

template <typename VertexStore>
typename BredthFirstTraversal<VertexStore>::self_type
    BredthFirstTraversal<VertexStore>::operator++(int dummy)
{
    (void)dummy;
    auto copy = *this;
    ++*this;
    return copy;
}

template <typename VertexStore>
typename BredthFirstTraversal<VertexStore>::reference
    BredthFirstTraversal<VertexStore>::operator*()
{
    return vertex_->second;
}

template <typename VertexStore>
typename BredthFirstTraversal<VertexStore>::pointer
    BredthFirstTraversal<VertexStore>::operator->()
{
    return &(vertex_->second);
}

template <typename VertexStore>
bool BredthFirstTraversal<VertexStore>::
operator==(const BredthFirstTraversal<VertexStore>& rhs) const
{
    return vertex_ == rhs.vertex_ && root_ == rhs.root_;
}

template <typename VertexStore>
bool BredthFirstTraversal<VertexStore>::
operator!=(const BredthFirstTraversal<VertexStore>& rhs) const
{
    return !(*this == rhs);
}

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
