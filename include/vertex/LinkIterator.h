#pragma once
#include <iterator>
#include <type_traits>
#include <utility>

namespace vertex
{

/** Follows links in Vertex by performing look-ups in a Container */
template <typename Container>
class LinkIterator : public std::iterator<std::input_iterator_tag,
                                          typename Container::mapped_type>
{
public:
    using container_type = Container;
    using vertex_iterator = typename container_type::iterator;
    using vertex_type = typename container_type::mapped_type;
    using child_iterator = typename vertex_type::iterator;
    using value_type = vertex_type;
    using self_type = LinkIterator<Container>;
    using reference = typename std::add_lvalue_reference<value_type>::type;
    using pointer = typename std::add_pointer<value_type>::type;

    LinkIterator(Container* vertices,
                 vertex_iterator vertex,
                 child_iterator child);

    self_type begin() const;
    self_type end() const;
    self_type& operator++();
    self_type operator++(int dummy);
    reference operator*();
    pointer operator->();

    bool operator!=(const self_type& rhs) const;
    bool operator==(const self_type& rhs) const;

private:
    Container* vertices_;
    vertex_iterator parent_;
    vertex_iterator child_;
    child_iterator chit_;
};

template <typename Container>
LinkIterator<Container>::LinkIterator(Container* vertices,
                                      vertex_iterator vertex,
                                      child_iterator child)
    : vertices_(vertices), parent_(std::move(vertex)), child_(vertices->end()),
      chit_(std::move(child))
{
}

template <typename Container>
typename LinkIterator<Container>::self_type
LinkIterator<Container>::begin() const
{
    return self_type(vertices_, parent_, parent_ == vertices_->end() ?
                                             child_iterator() :
                                             parent_->second.begin());
}

template <typename Container>
typename LinkIterator<Container>::self_type
LinkIterator<Container>::end() const
{
    return self_type(vertices_, parent_, child_iterator());
}

template <typename Container>
typename LinkIterator<Container>::self_type& LinkIterator<Container>::
operator++()
{
    if (++chit_ == parent_->second.end())
    {
        chit_ = child_iterator();
    }
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
    child_ = vertices_->find(chit_->key());
    return child_->second;
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
