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
    size_type size() const;

    /** Check whether the vertex has any children */
    bool empty() const;

    /** Remove all children from the Node */
    void clear();

    /** Exchanges the contents of the Node with those of other */
    void swap(Node& other);

    /** Insert a child vertex */
    std::pair<iterator, bool> insert(const_iterator pos, const Link& child);

    /** Insert a range of children */
    template <typename InputIt>
    iterator insert(iterator pos, InputIt first, InputIt last);

    /** Find a child vertex */
    iterator find(const Link& child);

    /** Find a child vertex */
    const_iterator find(const Link& child) const;

    /** Remove a child vertex */
    iterator erase(const_iterator child);

    /** Get the data associated with this vertex */
    const value_type& data() const;

    /** Set the data associated with this vertex */
    void data(const value_type& value);

    /** Compare the contents and children of a Node against another */
    bool operator==(const Node& rhs) const;

    /** Compare the contents and children of a Node against another */
    bool operator!=(const Node& rhs) const;

private:
    value_type data_;
    container_type children_;
};

template <typename Link, typename T>
Node<Link, T>::Node(value_type data) : data_(std::move(data))
{
}

template <typename Link, typename T>
typename Node<Link, T>::iterator Node<Link, T>::begin()
{
    return children_.begin();
}

template <typename Link, typename T>
typename Node<Link, T>::iterator Node<Link, T>::end()
{
    return children_.end();
}

template <typename Link, typename T>
typename Node<Link, T>::const_iterator Node<Link, T>::begin() const
{
    return children_.begin();
}

template <typename Link, typename T>
typename Node<Link, T>::const_iterator Node<Link, T>::end() const
{
    return children_.end();
}

template <typename Link, typename T>
typename Node<Link, T>::const_iterator Node<Link, T>::cbegin() const
{
    return children_.cbegin();
}

template <typename Link, typename T>
typename Node<Link, T>::const_iterator Node<Link, T>::cend() const
{
    return children_.cend();
}

template <typename Link, typename T>
typename Node<Link, T>::size_type Node<Link, T>::size() const
{
    return children_.size();
}

template <typename Link, typename T>
bool Node<Link, T>::empty() const
{
    return children_.empty();
}

template <typename Link, typename T>
void Node<Link, T>::clear()
{
    return children_.clear();
}

template <typename Link, typename T>
void Node<Link, T>::swap(Node& other)
{
    std::swap(children_, other.children_);
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
        it = children_.insert(pos, child);
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
    return children_.insert(pos, first, last);
}

template <typename Link, typename T>
typename Node<Link, T>::iterator Node<Link, T>::find(const Link& child)
{
    auto it = std::find(children_.begin(), children_.end(), child);
    return it;
}

template <typename Link, typename T>
typename Node<Link, T>::const_iterator
Node<Link, T>::find(const Link& child) const
{
    auto it = std::find(children_.cbegin(), children_.cend(), child);
    return it;
}

template <typename Link, typename T>
typename Node<Link, T>::iterator
Node<Link, T>::erase(typename Node<Link, T>::const_iterator child)
{
    return children_.erase(child);
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
bool Node<Link, T>::operator==(const Node<Link, T>& rhs) const
{
    return data_ == rhs.data_ && children_ == rhs.children_;
}

template <typename Link, typename T>
bool Node<Link, T>::operator!=(const Node<Link, T>& rhs) const
{
    return !(*this == rhs);
}

} // namespace vertex
