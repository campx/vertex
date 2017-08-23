#pragma once
#include <memory>

namespace vertex
{

/** An iterator adaptor which tracks the target vertex of an edge iterator */
template <typename EdgeIterator>
class EdgeTargetIterator
{
public:
    EdgeTargetIterator(EdgeIterator edge);
    EdgeTargetIterator(const EdgeTargetIterator&) = default;
    EdgeTargetIterator(EdgeTargetIterator&&) = default;
    EdgeTargetIterator& operator=(const EdgeTargetIterator&) = default;
    EdgeTargetIterator& operator=(EdgeTargetIterator&&) = default;

    using self_type = EdgeTargetIterator<EdgeIterator>;
    using value_type = typename EdgeIterator::vertex_store_type::mapped_type;
    using reference = value_type&;
    using pointer = value_type*;

    EdgeIterator edge() const;
    self_type begin() const;
    self_type end() const;
    self_type operator++();
    self_type operator++(int dummy);
    reference operator*();
    pointer operator->();

    bool operator!=(const self_type& rhs) const;
    bool operator==(const self_type& rhs) const;

private:
    using vertex_iterator = typename EdgeIterator::vertex_store_type::iterator;
    EdgeIterator edge_;
    EdgeIterator first_edge_;
    vertex_iterator vertex_;
};

template <typename EdgeIterator>
EdgeTargetIterator<EdgeIterator>::EdgeTargetIterator(EdgeIterator edge)
    : edge_(std::move(edge)), first_edge_(std::begin(edge_)),
      vertex_(edge_ == first_edge_ ? edge_.root() : edge_.vertex())
{
}

template <typename EdgeIterator>
EdgeIterator EdgeTargetIterator<EdgeIterator>::edge() const
{
    return edge_;
}

template <typename EdgeIterator>
typename EdgeTargetIterator<EdgeIterator>::self_type
EdgeTargetIterator<EdgeIterator>::begin() const
{
    return EdgeTargetIterator<EdgeIterator>(first_edge_);
}

template <typename EdgeIterator>
typename EdgeTargetIterator<EdgeIterator>::self_type
EdgeTargetIterator<EdgeIterator>::end() const
{
    return EdgeTargetIterator<EdgeIterator>(std::end(edge_));
}

template <typename EdgeIterator>
typename EdgeTargetIterator<EdgeIterator>::self_type
    EdgeTargetIterator<EdgeIterator>::operator++()
{
    if (vertex_ == edge_.root())
    {
        vertex_ = edge_.vertex();
    }
    else
    {
        vertex_ = (++edge_).vertex();
    }
    return *this;
}

template <typename EdgeIterator>
typename EdgeTargetIterator<EdgeIterator>::self_type
    EdgeTargetIterator<EdgeIterator>::operator++(int dummy)
{
    (void)dummy;
    auto self = *this;
    ++(*this);
    return self;
}

template <typename EdgeIterator>
typename EdgeTargetIterator<EdgeIterator>::reference
    EdgeTargetIterator<EdgeIterator>::operator*()
{
    return vertex_->second;
}

template <typename EdgeIterator>
typename EdgeTargetIterator<EdgeIterator>::pointer
    EdgeTargetIterator<EdgeIterator>::operator->()
{
    return &vertex_->second;
}

template <typename EdgeIterator>
bool EdgeTargetIterator<EdgeIterator>::
operator==(const EdgeTargetIterator<EdgeIterator>& rhs) const
{
    return edge_ == rhs.edge_;
}

template <typename EdgeIterator>
bool EdgeTargetIterator<EdgeIterator>::
operator!=(const EdgeTargetIterator<EdgeIterator>& rhs) const
{
    return !(*this == rhs);
}

} // namespace vertex
