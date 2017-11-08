#pragma once
#include <utility>

namespace vertex
{

/** A predicate which always returns either true or false regardless of the
 * input */
template <typename VertexMap, bool Result>
struct NullaryPredicate
{
    using result_type = bool;
    using argument_type = std::pair<typename VertexMap::key_type,
                                    typename VertexMap::key_type>;
    bool operator()(const argument_type& value) const;
};

template <typename VertexMap, bool Result>
bool NullaryPredicate<VertexMap, Result>::
operator()(const typename NullaryPredicate<VertexMap, Result>::argument_type&
               value) const
{
    (void)value;
    return Result;
}

/** @TODO Implement traversal routine in this class in terms of Impl methods,
 * implement SkipIterator which evaluates a predicate on increment and skips
 * over elements which do not pass
 */
template <typename VertexMap,
          typename Impl,
          typename Predicate = NullaryPredicate<VertexMap, true>>
class Traversal
{
public:
    using vertex_store_type = VertexMap;
    using vertex_iterator = typename VertexMap::iterator;
    using vertex_type = typename VertexMap::mapped_type;
    using key_type = typename VertexMap::key_type;
    using edge_type = std::pair<key_type, key_type>;
    using predicate_type = Predicate;

    using value_type = Impl;
    using self_type = Traversal<VertexMap, Impl, Predicate>;
    using reference = Impl&;
    using pointer = Impl*;

    Traversal(VertexMap* vertices,
              typename VertexMap::iterator root,
              Predicate predicate = Predicate{});

    VertexMap* vertices() const;
    const vertex_iterator& root() const;
    const vertex_iterator& position() const;
    const vertex_type& vertex() const;
    const edge_type& edge() const;
    const predicate_type& predicate() const;
    bool isTraversible(const edge_type& value) const;

    Impl begin() const;
    Impl end() const;
    Impl& operator++();
    Impl operator++(int dummy);
    reference operator*();
    pointer operator->();

    bool operator!=(const self_type& rhs) const;
    bool operator==(const self_type& rhs) const;

protected:
    void position(const vertex_iterator& value);
    void vertex(const vertex_type& value);
    void edge(const edge_type& value);

private:
    VertexMap* vertices_;
    vertex_iterator root_;
    vertex_iterator position_;
    vertex_type vertex_;
    edge_type edge_;
    Predicate predicate_;
};

template <typename VertexMap, typename Impl, typename Predicate>
Traversal<VertexMap, Impl, Predicate>::Traversal(
    VertexMap* vertices,
    typename VertexMap::iterator root,
    Predicate predicate)
    : vertices_(vertices), root_(std::move(root)), position_(root_),
      vertex_(position_ == vertices_->end() ? vertex_type{} :
                                              position_->second),
      edge_(std::make_pair(key_type{},
                           position_ == vertices_->end() ? key_type{} :
                                                           position_->first)),
      predicate_(std::move(predicate))
{
}

template <typename VertexMap, typename Impl, typename Predicate>
VertexMap* Traversal<VertexMap, Impl, Predicate>::vertices() const
{
    return vertices_;
}

template <typename VertexMap, typename Impl, typename Predicate>
const typename Traversal<VertexMap, Impl, Predicate>::vertex_iterator&
Traversal<VertexMap, Impl, Predicate>::root() const
{
    return root_;
}

template <typename VertexMap, typename Impl, typename Predicate>
const typename Traversal<VertexMap, Impl, Predicate>::vertex_iterator&
Traversal<VertexMap, Impl, Predicate>::position() const
{
    return position_;
}

template <typename VertexMap, typename Impl, typename Predicate>
const typename Traversal<VertexMap, Impl, Predicate>::vertex_type&
Traversal<VertexMap, Impl, Predicate>::vertex() const
{
    return vertex_;
}

template <typename VertexMap, typename Impl, typename Predicate>
const typename Traversal<VertexMap, Impl, Predicate>::edge_type&
Traversal<VertexMap, Impl, Predicate>::edge() const
{
    return edge_;
}

template <typename VertexMap, typename Impl, typename Predicate>
const typename Traversal<VertexMap, Impl, Predicate>::predicate_type&
Traversal<VertexMap, Impl, Predicate>::predicate() const
{
    return predicate_;
}

template <typename VertexMap, typename Impl, typename Predicate>
void Traversal<VertexMap, Impl, Predicate>::vertex(
    const typename Traversal<VertexMap, Impl, Predicate>::vertex_type& value)
{
    vertex_ = value;
}

template <typename VertexMap, typename Impl, typename Predicate>
void Traversal<VertexMap, Impl, Predicate>::position(
    const typename Traversal<VertexMap, Impl, Predicate>::vertex_iterator&
        value)
{
    position_ = value;
}

template <typename VertexMap, typename Impl, typename Predicate>
void Traversal<VertexMap, Impl, Predicate>::edge(
    const typename Traversal<VertexMap, Impl, Predicate>::edge_type& value)
{
    edge_ = value;
}

template <typename VertexMap, typename Impl, typename Predicate>
bool Traversal<VertexMap, Impl, Predicate>::isTraversible(
    const typename Traversal<VertexMap, Impl, Predicate>::edge_type& value)
    const
{
    return predicate_(value);
}

template <typename VertexMap, typename Impl, typename Predicate>
Impl Traversal<VertexMap, Impl, Predicate>::begin() const
{
    return Impl(vertices(), root(), predicate());
}

template <typename VertexMap, typename Impl, typename Predicate>
Impl Traversal<VertexMap, Impl, Predicate>::end() const
{
    auto result = Impl(vertices(), root(), predicate());
    result.position(vertices()->end());
    return result;
}

template <typename VertexMap, typename Impl, typename Predicate>
Impl& Traversal<VertexMap, Impl, Predicate>::operator++()
{
    auto pImpl = static_cast<Impl*>(this);
    if (!pImpl->next())
    {
        position(vertices()->end());
    }
    return *pImpl;
}

template <typename VertexMap, typename Impl, typename Predicate>
Impl Traversal<VertexMap, Impl, Predicate>::operator++(int dummy)
{
    (void)dummy;
    auto pImpl = static_cast<Impl*>(this);
    auto copy = *pImpl;
    ++*this;
    return copy;
}

template <typename VertexMap, typename Impl, typename Predicate>
Impl& Traversal<VertexMap, Impl, Predicate>::operator*()
{
    auto pImpl = static_cast<Impl*>(this);
    return *pImpl;
}

template <typename VertexMap, typename Impl, typename Predicate>
Impl* Traversal<VertexMap, Impl, Predicate>::operator->()
{
    auto pImpl = static_cast<Impl*>(this);
    return pImpl;
}

template <typename VertexMap, typename Impl, typename Predicate>
bool Traversal<VertexMap, Impl, Predicate>::
operator==(const Traversal<VertexMap, Impl, Predicate>& rhs) const
{
    return position() == rhs.position() && root() == rhs.root();
}

template <typename VertexMap, typename Impl, typename Predicate>
bool Traversal<VertexMap, Impl, Predicate>::
operator!=(const Traversal<VertexMap, Impl, Predicate>& rhs) const
{
    return !(*this == rhs);
}

} // namespace vertex
