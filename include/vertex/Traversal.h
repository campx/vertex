#pragma once
#include <utility>
#include <vertex/Predicate.h>

namespace vertex
{

/** @TODO Implement traversal routine in this class in terms of Impl methods,
 * implement SkipIterator which evaluates a predicate on increment and skips
 * over elements which do not pass
 * @TODO Reverse iterators, prev method
 */
template <typename Container,
          typename Impl,
          typename Predicate = NullaryPredicate<Container, true>>
class Traversal
{
public:
    using container_type = Container;
    using vertex_iterator = typename Container::const_iterator;
    using vertex_type = typename Container::mapped_type;
    using child_iterator = typename vertex_type::iterator;
    using key_type = typename Container::key_type;
    using edge_type = std::pair<key_type, key_type>;
    using predicate_type = Predicate;

    using value_type = Impl;
    using self_type = Traversal<Container, Impl, Predicate>;
    using reference = Impl&;
    using pointer = Impl*;

    Traversal(const Container& vertices,
              typename Container::const_iterator root,
              Predicate predicate = Predicate{});

    const Container& vertices() const;
    const vertex_iterator& root() const;
    const vertex_iterator& position() const;
    const child_iterator& child() const;
    const vertex_type& vertex() const;
    const edge_type& edge() const;
    const predicate_type& predicate() const;
    bool isTraversible(const edge_type& value);

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
    void child(const child_iterator& value);
    void vertex(const vertex_type& value);
    void edge(const edge_type& value);

private:
    const Container* vertices_;
    vertex_iterator root_;
    vertex_iterator position_;
    child_iterator child_;
    vertex_type vertex_;
    edge_type edge_;
    Predicate predicate_;
};

template <typename Container, typename Impl, typename Predicate>
Traversal<Container, Impl, Predicate>::Traversal(
    const Container& vertices,
    typename Container::const_iterator root,
    Predicate predicate)
    : vertices_(&vertices), root_(std::move(root)), position_(root_),
      vertex_(position_ == vertices_->end() ? vertex_type{} :
                                              position_->second),
      edge_(std::make_pair(key_type{},
                           position_ == vertices_->end() ? key_type{} :
                                                           position_->first)),
      predicate_(std::move(predicate))
{
}

template <typename Container, typename Impl, typename Predicate>
const Container& Traversal<Container, Impl, Predicate>::vertices() const
{
    return *vertices_;
}

template <typename Container, typename Impl, typename Predicate>
const typename Traversal<Container, Impl, Predicate>::vertex_iterator&
Traversal<Container, Impl, Predicate>::root() const
{
    return root_;
}

template <typename Container, typename Impl, typename Predicate>
const typename Traversal<Container, Impl, Predicate>::vertex_iterator&
Traversal<Container, Impl, Predicate>::position() const
{
    return position_;
}

template <typename Container, typename Impl, typename Predicate>
const typename Traversal<Container, Impl, Predicate>::vertex_type&
Traversal<Container, Impl, Predicate>::vertex() const
{
    return vertex_;
}

template <typename Container, typename Impl, typename Predicate>
const typename Traversal<Container, Impl, Predicate>::edge_type&
Traversal<Container, Impl, Predicate>::edge() const
{
    return edge_;
}

template <typename Container, typename Impl, typename Predicate>
const typename Traversal<Container, Impl, Predicate>::predicate_type&
Traversal<Container, Impl, Predicate>::predicate() const
{
    return predicate_;
}

template <typename Container, typename Impl, typename Predicate>
void Traversal<Container, Impl, Predicate>::vertex(
    const typename Traversal<Container, Impl, Predicate>::vertex_type& value)
{
    vertex_ = value;
}

template <typename Container, typename Impl, typename Predicate>
void Traversal<Container, Impl, Predicate>::position(
    const typename Traversal<Container, Impl, Predicate>::vertex_iterator&
        value)
{
    position_ = value;
}

template <typename Container, typename Impl, typename Predicate>
void Traversal<Container, Impl, Predicate>::edge(
    const typename Traversal<Container, Impl, Predicate>::edge_type& value)
{
    edge_ = value;
}

template <typename Container, typename Impl, typename Predicate>
bool Traversal<Container, Impl, Predicate>::isTraversible(
    const typename Traversal<Container, Impl, Predicate>::edge_type& value)
{
    return predicate_(value);
}

template <typename Container, typename Impl, typename Predicate>
Impl Traversal<Container, Impl, Predicate>::begin() const
{
    return Impl(vertices(), root(), predicate());
}

template <typename Container, typename Impl, typename Predicate>
Impl Traversal<Container, Impl, Predicate>::end() const
{
    auto result = Impl(vertices(), root(), predicate());
    result.position(vertices().end());
    return result;
}

template <typename Container, typename Impl, typename Predicate>
Impl& Traversal<Container, Impl, Predicate>::operator++()
{
    auto pImpl = static_cast<Impl*>(this);
    if (!pImpl->next())
    {
        position(vertices().end());
    }
    return *pImpl;
}

template <typename Container, typename Impl, typename Predicate>
Impl Traversal<Container, Impl, Predicate>::operator++(int dummy)
{
    (void)dummy;
    auto pImpl = static_cast<Impl*>(this);
    auto copy = *pImpl;
    ++*this;
    return copy;
}

template <typename Container, typename Impl, typename Predicate>
Impl& Traversal<Container, Impl, Predicate>::operator*()
{
    auto pImpl = static_cast<Impl*>(this);
    return *pImpl;
}

template <typename Container, typename Impl, typename Predicate>
Impl* Traversal<Container, Impl, Predicate>::operator->()
{
    auto pImpl = static_cast<Impl*>(this);
    return pImpl;
}

template <typename Container, typename Impl, typename Predicate>
bool Traversal<Container, Impl, Predicate>::
operator==(const Traversal<Container, Impl, Predicate>& rhs) const
{
    return position() == rhs.position() && root() == rhs.root();
}

template <typename Container, typename Impl, typename Predicate>
bool Traversal<Container, Impl, Predicate>::
operator!=(const Traversal<Container, Impl, Predicate>& rhs) const
{
    return !(*this == rhs);
}

} // namespace vertex
