#pragma once
#include <functional>
#include <memory>
#include <queue>

namespace vertex
{

template <typename Traversal, typename Accessor>
class Iterator
{
public:
    using vertex_store_type = typename Traversal::vertex_store_type;
    using vertex_iterator = typename vertex_store_type::iterator;
    using value_type = typename std::result_of<Accessor(Traversal)>::type;

    Iterator(std::shared_ptr<Traversal> traversal,
             Accessor accessor = Accessor());
    Iterator(vertex_store_type* vertices,
             vertex_iterator vertex,
             typename Traversal::predicate_type predicate =
                 typename Traversal::predicate_type{},
             Accessor accessor = Accessor());
    Iterator(const Iterator&) = default;
    Iterator(Iterator&&) = default;
    Iterator& operator=(const Iterator&) = default;
    Iterator& operator=(Iterator&&) = default;

    using self_type = Iterator<Traversal, Accessor>;
    using reference = typename std::add_lvalue_reference<value_type>::type;
    using pointer = typename std::add_pointer<value_type>::type;

    const std::shared_ptr<Traversal>& traversal() const;

    self_type begin() const;
    self_type end() const;
    self_type& operator++();
    self_type& operator++(int dummy);
    reference operator*();
    pointer operator->();

    bool operator!=(const self_type& rhs) const;
    bool operator==(const self_type& rhs) const;

private:
    std::shared_ptr<Traversal> traversal_;
    Accessor accessor_;
};

template <typename Traversal, typename Accessor>
Iterator<Traversal, Accessor>::Iterator(std::shared_ptr<Traversal> traversal,
                                        Accessor accessor)
    : traversal_(std::move(traversal)), accessor_(std::move(accessor))
{
}

template <typename Traversal, typename Accessor>
Iterator<Traversal, Accessor>::Iterator(
    vertex_store_type* vertices,
    typename vertex_store_type::iterator vertex,
    typename Traversal::predicate_type predicate,
    Accessor accessor)
    : Iterator(std::make_shared<Traversal>(
                   vertices, std::move(vertex), std::move(predicate)),
               std::move(accessor))
{
}

template <typename Traversal, typename Accessor>
const std::shared_ptr<Traversal>&
Iterator<Traversal, Accessor>::traversal() const
{
    return traversal_;
}

template <typename Traversal, typename Accessor>
Iterator<Traversal, Accessor> Iterator<Traversal, Accessor>::begin() const
{
    return Iterator<Traversal, Accessor>(traversal_->vertices(),
                                         traversal_->root(),
                                         traversal_->predicate(), accessor_);
}

template <typename Traversal, typename Accessor>
Iterator<Traversal, Accessor> Iterator<Traversal, Accessor>::end() const
{
    return Iterator<Traversal, Accessor>(traversal_->vertices(),
                                         traversal_->vertices()->end(),
                                         traversal_->predicate(), accessor_);
}

template <typename Traversal, typename Accessor>
Iterator<Traversal, Accessor>& Iterator<Traversal, Accessor>::operator++()
{
    if (!traversal_->advance())
    {
        *this = end();
    }
    return *this;
}

template <typename Traversal, typename Accessor>
Iterator<Traversal, Accessor>& Iterator<Traversal, Accessor>::
operator++(int dummy)
{
    (void)dummy;
    auto copy = *this;
    ++*this;
    return copy;
}

template <typename Traversal, typename Accessor>
typename Iterator<Traversal, Accessor>::reference
    Iterator<Traversal, Accessor>::operator*()
{
    return accessor_(*traversal_);
}

template <typename Traversal, typename Accessor>
typename Iterator<Traversal, Accessor>::pointer Iterator<Traversal, Accessor>::
operator->()
{
    return &accessor_(*traversal_);
}

template <typename Traversal, typename Accessor>
bool Iterator<Traversal, Accessor>::
operator==(const Iterator<Traversal, Accessor>& rhs) const
{
    return traversal_->position() == rhs.traversal_->position() &&
           traversal_->root() == rhs.traversal_->root();
}

template <typename Traversal, typename Accessor>
bool Iterator<Traversal, Accessor>::
operator!=(const Iterator<Traversal, Accessor>& rhs) const
{
    return !(*this == rhs);
}

template <typename Traversal, typename Accessor>
Iterator<Traversal, Accessor>
make_iterator(const std::shared_ptr<Traversal>& traversal,
              const Accessor& accessor)
{
    return Iterator<Traversal, Accessor>(traversal, accessor);
}

} // namespace vertex
