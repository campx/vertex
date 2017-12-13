#pragma once
#include <iterator>
#include <type_traits>
#include <utility>

namespace vertex
{

/** Follows links in Node by performing look-ups in a Container */
template <typename Container>
class LinkIterator : public std::iterator<std::input_iterator_tag,
                                          typename Container::mapped_type>
{
public:
    using container_type = Container;
    using vertex_iterator = typename container_type::const_iterator;
    using vertex_type = typename container_type::mapped_type;
    using child_iterator = typename vertex_type::const_iterator;
    using value_type = typename Container::value_type;
    using self_type = LinkIterator<Container>;
    using reference = const value_type&;
    using pointer = const value_type*;

    LinkIterator(const Container* vertices,
                 const vertex_type* parent,
                 child_iterator child);

    child_iterator position() const;
    self_type begin() const;
    self_type end() const;
    self_type& operator++();
    self_type operator++(int dummy);
    reference operator*();
    pointer operator->();

    bool operator!=(const self_type& rhs) const;
    bool operator==(const self_type& rhs) const;

private:
    const Container* vertices_;
    const vertex_type* parent_;
    vertex_iterator child_;
    child_iterator chit_;
};

template <typename Container>
LinkIterator<Container>::LinkIterator(const Container* vertices,
                                      const vertex_type* parent,
                                      child_iterator child)
    : vertices_(vertices), parent_(parent), child_(vertices->cend()),
      chit_(std::move(child))
{
}

template <typename Container>
typename LinkIterator<Container>::child_iterator
LinkIterator<Container>::position() const
{
    return chit_;
}

template <typename Container>
typename LinkIterator<Container>::self_type
LinkIterator<Container>::begin() const
{
    return self_type(vertices_, parent_, parent_->begin());
}

template <typename Container>
typename LinkIterator<Container>::self_type
LinkIterator<Container>::end() const
{
    return self_type(vertices_, parent_, parent_->end());
}

template <typename Container>
typename LinkIterator<Container>::self_type& LinkIterator<Container>::
operator++()
{
    ++chit_;
    return *this;
}

template <typename Container>
typename LinkIterator<Container>::self_type LinkIterator<Container>::
operator++(int dummy)
{
    (void)dummy;
    auto copy = *this;
    ++*this;
    return copy;
}

template <typename Container>
typename LinkIterator<Container>::reference LinkIterator<Container>::
operator*()
{
    child_ = vertices_->find(chit_->id());
    return *child_;
}

template <typename Container>
typename LinkIterator<Container>::pointer LinkIterator<Container>::operator->()
{
    return &(*this);
}

template <typename Container>
bool LinkIterator<Container>::operator==(const self_type& rhs) const
{
    return vertices_ == rhs.vertices_ && parent_ == rhs.parent_ &&
           chit_ == rhs.chit_;
}

template <typename Container>
bool LinkIterator<Container>::operator!=(const self_type& rhs) const
{
    return !(*this == rhs);
}

} // namespace vertex
