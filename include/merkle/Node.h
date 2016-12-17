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
        : data_(std::move(data)), is_dirty_(true)
    {
    }

    /** Set the data associated with this node */
    element_type data() const { return data_; }

    /** Get the data associated with this node */
    void data(const element_type& value) { data_ = value; }

    /** Get the hash of this node */
    value_type hash() const;

    /** Get an iterator to the first child link of the node */
    const_iterator begin() const { return children_.cbegin(); }

    /** Get an iterator one position past the last child link of the node */
    const_iterator end() const { return children_.cend(); }

    const container_type& children() const { return children_; }

    /** Insert a child node */
    const_iterator insert(const value_type& child);

    /** Insert a hash-node pair at the given iterator position. The position
     * must be valid */
    const_iterator insert(const_iterator pos, const value_type& value);

    const_iterator erase(const_iterator pos);

    const_iterator erase(const value_type& child);

    const_iterator find(const value_type& value) const;

    size_type size() const { return children_.size(); }

    bool empty() const { return children_.empty(); }

    bool operator==(const Node& rhs) const;

    bool operator!=(const Node& rhs) const;

    bool operator>(const Node& rhs) const;

    bool operator<(const Node& rhs) const;

private:
    element_type data_;
    mutable value_type hash_;
    container_type children_;
    mutable bool is_dirty_;
};

template <typename T, typename Link, typename Impl>
typename Node<T, Link, Impl>::value_type Node<T, Link, Impl>::hash() const
{
    if (is_dirty_)
    {
        hash_ = static_cast<const Impl*>(this)->computeHash();
        is_dirty_ = false;
    }
    return hash_;
}

template <typename T, typename Link, typename Impl>
typename Node<T, Link, Impl>::const_iterator Node<T, Link, Impl>::insert(
    const typename Node<T, Link, Impl>::value_type& child)
{
    return insert(children().end(), child);
}

template <typename T, typename Link, typename Impl>
typename Node<T, Link, Impl>::const_iterator
Node<T, Link, Impl>::insert(typename Node<T, Link, Impl>::const_iterator pos,
                            const Node<T, Link, Impl>::value_type& value)
{
    // Don't insert if value already exists
    auto child = find(value);
    if (children().end() == child)
    {
        child = children_.insert(pos, value);
        is_dirty_ = true;
    }
    return child;
}

template <typename T, typename Link, typename Impl>
typename Node<T, Link, Impl>::const_iterator Node<T, Link, Impl>::find(
    const typename Node<T, Link, Impl>::value_type& value) const
{
    return std::find(children().begin(), children().end(), value);
}

template <typename T, typename Link, typename Impl>
typename Node<T, Link, Impl>::const_iterator
Node<T, Link, Impl>::erase(typename Node<T, Link, Impl>::const_iterator pos)
{
    is_dirty_ = true;
    return children_.erase(pos);
}

template <typename T, typename Link, typename Impl>
typename Node<T, Link, Impl>::const_iterator
Node<T, Link, Impl>::erase(const Link& value)
{
    auto result = find(value);
    if (result != end())
    {
        result = erase(result);
    }
    return result;
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
