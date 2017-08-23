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
class BfsEdgeIterator
{
public:
    BfsEdgeIterator(VertexStore* vertices,
                    typename VertexStore::iterator vertex);
    BfsEdgeIterator(const BfsEdgeIterator&) = default;
    BfsEdgeIterator(BfsEdgeIterator&&) = default;
    BfsEdgeIterator& operator=(const BfsEdgeIterator&) = default;
    BfsEdgeIterator& operator=(BfsEdgeIterator&&) = default;

    using vertex_store_type = VertexStore;
    using vertex_iterator = typename VertexStore::iterator;
    using self_type = BfsEdgeIterator<VertexStore>;
    using value_type = std::pair<typename VertexStore::key_type,
                                 typename VertexStore::key_type>;
    using reference = value_type&;
    using pointer = value_type*;

    vertex_iterator root() const;
    vertex_iterator vertex() const;
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
    using vertex_type = typename VertexStore::mapped_type;
    using child_iterator = typename vertex_type::iterator;
    using key_type = typename VertexStore::key_type;
    std::queue<value_type> to_visit_;
    std::pair<typename VertexStore::key_type, typename VertexStore::key_type>
        edge_;
    VertexStore* vertices_;
    vertex_iterator root_;
    vertex_iterator vertex_;
};

template <typename VertexStore>
BfsEdgeIterator<VertexStore>::BfsEdgeIterator(
    VertexStore* vertices, typename VertexStore::iterator vertex)
    : vertices_(vertices), root_(std::move(vertex)), vertex_(root_)
{
    enqueue_children();
}

template <typename VertexStore>
typename BfsEdgeIterator<VertexStore>::vertex_iterator
BfsEdgeIterator<VertexStore>::root() const
{
    return root_;
}

template <typename VertexStore>
typename BfsEdgeIterator<VertexStore>::vertex_iterator
BfsEdgeIterator<VertexStore>::vertex() const
{
    return vertex_;
}

template <typename VertexStore>
void BfsEdgeIterator<VertexStore>::enqueue_children()
{
    if (vertex_ != vertices_->end())
    {
        for (const auto& child : vertex_->second)
        {
            to_visit_.push(std::make_pair(vertex_->first, child));
        }
    }
    if (!to_visit_.empty())
    {
        edge_ = to_visit_.front();
        to_visit_.pop();
        vertex_ = vertices_->find(edge_.second);
    }
}

template <typename VertexStore>
typename BfsEdgeIterator<VertexStore>::self_type
BfsEdgeIterator<VertexStore>::begin() const
{
    return BfsEdgeIterator(vertices_, root_);
}

template <typename VertexStore>
typename BfsEdgeIterator<VertexStore>::self_type
BfsEdgeIterator<VertexStore>::end() const
{
    return BfsEdgeIterator(vertices_, vertices_->end());
}

template <typename VertexStore>
typename BfsEdgeIterator<VertexStore>::self_type BfsEdgeIterator<VertexStore>::
operator++()
{
    if (to_visit_.empty())
    {
        *this = end();
    }
    else
    {
        enqueue_children();
    }
    return *this;
}

template <typename VertexStore>
typename BfsEdgeIterator<VertexStore>::self_type BfsEdgeIterator<VertexStore>::
operator++(int dummy)
{
    (void)dummy;
    auto copy = *this;
    ++*this;
    return copy;
}

template <typename VertexStore>
typename BfsEdgeIterator<VertexStore>::reference BfsEdgeIterator<VertexStore>::
operator*()
{
    return edge_;
}

template <typename VertexStore>
typename BfsEdgeIterator<VertexStore>::pointer BfsEdgeIterator<VertexStore>::
operator->()
{
    return &edge_;
}

template <typename VertexStore>
bool BfsEdgeIterator<VertexStore>::
operator==(const BfsEdgeIterator<VertexStore>& rhs) const
{
    return edge_ == rhs.edge_ && root_ == rhs.root_;
}

template <typename VertexStore>
bool BfsEdgeIterator<VertexStore>::
operator!=(const BfsEdgeIterator<VertexStore>& rhs) const
{
    return !(*this == rhs);
}

} // namespace vertex
