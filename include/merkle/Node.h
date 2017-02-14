#pragma once
#include <algorithm>
#include <vector>

namespace merkle
{

/** Node contains links to children, which can be iterated through */
template <typename T, typename Link, typename Impl>
class Node
{
public:
    /** Type of data stored by a Node */
    using element_type = T;
    /** Result type of hashing a Node */
    using value_type = Link;

    using container_type = std::vector<value_type>;
    using const_iterator = typename container_type::const_iterator;
    using size_type = typename container_type::size_type;

    explicit Node(element_type data = element_type())
        : data_(std::move(data)), children_()
    {
    }

    template <typename Iter = typename container_type::const_iterator>
    Node(Iter first, Iter last, element_type data = element_type())
        : data_(std::move(data)), children_(first, last)
    {
    }

    /** Get the data associated with this node */
    element_type data() const
    {
        return data_;
    }

    /** Get the hash of this node */
    value_type hash() const;

    /** Get an iterator to the first child link of the node */
    const_iterator begin() const
    {
        return children_.cbegin();
    }

    /** Get an iterator one position past the last child link of the node */
    const_iterator end() const
    {
        return children_.cend();
    }

    const_iterator find(const value_type& value) const;

    size_type size() const
    {
        return children_.size();
    }

    bool empty() const
    {
        return children_.empty();
    }

    bool operator==(const Node& rhs) const;

    bool operator!=(const Node& rhs) const;

    bool operator>(const Node& rhs) const;

    bool operator<(const Node& rhs) const;

private:
    element_type data_;
    container_type children_;
};

template <typename T, typename Link, typename Impl>
typename Node<T, Link, Impl>::value_type Node<T, Link, Impl>::hash() const
{
    return static_cast<const Impl*>(this)->computeHash();
}

template <typename T, typename Link, typename Impl>
typename Node<T, Link, Impl>::const_iterator Node<T, Link, Impl>::find(
    const typename Node<T, Link, Impl>::value_type& value) const
{
    return std::find(begin(), end(), value);
}

template <typename T, typename Link, typename Impl>
bool Node<T, Link, Impl>::operator==(const Node<T, Link, Impl>& rhs) const
{
    return hash() == rhs.hash();
}

template <typename T, typename Link, typename Impl>
bool Node<T, Link, Impl>::operator!=(const Node<T, Link, Impl>& rhs) const
{
    return hash() != rhs.hash();
}

template <typename T, typename Link, typename Impl>
bool Node<T, Link, Impl>::operator<(const Node<T, Link, Impl>& rhs) const
{
    return hash() < rhs.hash();
}

template <typename T, typename Link, typename Impl>
bool Node<T, Link, Impl>::operator>(const Node<T, Link, Impl>& rhs) const
{
    return hash() > rhs.hash();
}

} // namespace merkle
