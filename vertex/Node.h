#pragma once
#include <algorithm>
#include <vector>

namespace vertex
{

/** A node in a tree, which has data of type T, and a collection of Link
 * pointers to child nodes.
 * Provides a set-like interface for adding and removing children */
template <typename Link, typename T>
class Node
{
public:
    using link_type = Link;
    using container_type = std::vector<link_type>;

    using value_type = T;
    using size_type = typename container_type::size_type;
    using difference_type = typename container_type::difference_type;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using pointer = typename container_type::pointer;
    using const_pointer = typename container_type::const_pointer;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using reverse_iterator = typename container_type::reverse_iterator;
    using const_reverse_iterator =
        typename container_type::const_reverse_iterator;

    /** Create a Node with the given data value */
    explicit Node(value_type data = value_type());

    Node(const Node&) = default;
    Node(Node&&) = default;
    Node& operator=(const Node&) = default;
    Node& operator=(Node&&) = default;

    /** Return an iterator to the first child Link */
    iterator begin();
    /** Return an iterator to the first child Link */
    const_iterator begin() const;
    /** Return an iterator to the first child Link */
    const_iterator cbegin() const;
    /** Return an iterator to the element following the last child Link */
    iterator end();
    /** Return an iterator to the element following the last child Link */
    const_iterator end() const;
    /** Return an iterator to the element following the last child Link */
    const_iterator cend() const;

    /** Get the number of children */
    size_type length() const;

    /** Check whether the node has any children */
    bool empty() const;

    /** Remove all children from the Node */
    void clear();

    /** Exchanges the contents of the Node with those of other */
    void swap(Node& other);

    /** Insert a child node */
    std::pair<iterator, bool> insert(const_iterator pos, const Link& child);

    /** Insert a range of children */
    template <typename InputIt>
    iterator insert(iterator pos, InputIt first, InputIt last);

    /** Find a child node */
    iterator find(const Link& child);

    /** Find a child node */
    const_iterator find(const Link& child) const;

    /** Remove a child node */
    iterator erase(const_iterator child);

    /** Get the out-bound links from this node */
    const container_type& links() const;

    /** Set the out-bound links for this node */
    void links(const container_type& value);

    /** Get the data associated with this node */
    const value_type& data() const;

    /** Set the data associated with this node */
    void data(const value_type& value);

    /** Provide read-write access to the data */
    value_type& data();

    /** Compare the contents and children of a Node against another */
    bool operator==(const Node& rhs) const;

    /** Compare the contents and children of a Node against another */
    bool operator!=(const Node& rhs) const;

private:
    value_type data_;
    container_type links_;
};

template <typename Link, typename T>
Node<Link, T>::Node(value_type data) : data_(std::move(data))
{
}

template <typename Link, typename T>
typename Node<Link, T>::iterator Node<Link, T>::begin()
{
    return links_.begin();
}

template <typename Link, typename T>
typename Node<Link, T>::iterator Node<Link, T>::end()
{
    return links_.end();
}

template <typename Link, typename T>
typename Node<Link, T>::const_iterator Node<Link, T>::begin() const
{
    return links_.begin();
}

template <typename Link, typename T>
typename Node<Link, T>::const_iterator Node<Link, T>::end() const
{
    return links_.end();
}

template <typename Link, typename T>
typename Node<Link, T>::const_iterator Node<Link, T>::cbegin() const
{
    return links_.cbegin();
}

template <typename Link, typename T>
typename Node<Link, T>::const_iterator Node<Link, T>::cend() const
{
    return links_.cend();
}

template <typename Link, typename T>
typename Node<Link, T>::size_type Node<Link, T>::length() const
{
    return links_.size();
}

template <typename Link, typename T>
bool Node<Link, T>::empty() const
{
    return links_.empty();
}

template <typename Link, typename T>
void Node<Link, T>::clear()
{
    return links_.clear();
}

template <typename Link, typename T>
void Node<Link, T>::swap(Node& other)
{
    std::swap(links_, other.links_);
    std::swap(data_, other.data_);
}

template <typename Link, typename T>
std::pair<typename Node<Link, T>::iterator, bool>
Node<Link, T>::insert(typename Node<Link, T>::const_iterator pos,
                      const Link& child)
{
    auto it = find(child);
    auto do_insert = it == end();
    if (do_insert)
    {
        it = links_.insert(pos, child);
    }
    auto result = std::make_pair(it, do_insert);
    return result;
}

template <typename Link, typename T>
template <typename InputIt>
typename Node<Link, T>::iterator
Node<Link, T>::insert(typename Node<Link, T>::iterator pos,
                      InputIt first,
                      InputIt last)
{
    return links_.insert(pos, first, last);
}

template <typename Link, typename T>
typename Node<Link, T>::iterator Node<Link, T>::find(const Link& child)
{
    auto it = std::find(links_.begin(), links_.end(), child);
    return it;
}

template <typename Link, typename T>
typename Node<Link, T>::const_iterator
Node<Link, T>::find(const Link& child) const
{
    auto it = std::find(links_.cbegin(), links_.cend(), child);
    return it;
}

template <typename Link, typename T>
typename Node<Link, T>::iterator
Node<Link, T>::erase(typename Node<Link, T>::const_iterator child)
{
    return links_.erase(child);
}

template <typename Link, typename T>
const typename Node<Link, T>::container_type& Node<Link, T>::links() const
{
    return links_;
}

template <typename Link, typename T>
void Node<Link, T>::links(const typename Node<Link, T>::container_type& value)
{
    links_ = value;
}

template <typename Link, typename T>
const typename Node<Link, T>::value_type& Node<Link, T>::data() const
{
    return data_;
}

template <typename Link, typename T>
void Node<Link, T>::data(const typename Node<Link, T>::value_type& value)
{
    data_ = value;
}

template <typename Link, typename T>
typename Node<Link, T>::value_type& Node<Link, T>::data()
{
    return data_;
}

template <typename Link, typename T>
bool Node<Link, T>::operator==(const Node<Link, T>& rhs) const
{
    return data_ == rhs.data_ && links_ == rhs.links_;
}

template <typename Link, typename T>
bool Node<Link, T>::operator!=(const Node<Link, T>& rhs) const
{
    return !(*this == rhs);
}

} // namespace vertex
