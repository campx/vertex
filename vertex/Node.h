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
    using container_type = std::vector<Link>;
    using value_type = T;

    /** Create a Node with the given data value */
    explicit Node(value_type data = value_type(),
                  container_type links = container_type());

    Node(const Node&) = default;
    Node(Node&&) = default;
    Node& operator=(const Node&) = default;
    Node& operator=(Node&&) = default;

    /** Exchanges the contents of the Node with those of other */
    void swap(Node& other);

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

    /** Provide read-write access to the links */
    container_type& links();

    /** Compare the contents and children of a Node against another */
    bool operator==(const Node& rhs) const;

    /** Compare the contents and children of a Node against another */
    bool operator!=(const Node& rhs) const;

private:
    value_type data_;
    container_type links_;
};

template <typename Link, typename T>
Node<Link, T>::Node(value_type data, container_type links)
    : data_(std::move(data)), links_(std::move(links))
{
}

template <typename Link, typename T>
void Node<Link, T>::swap(Node& other)
{
    std::swap(links_, other.links_);
    std::swap(data_, other.data_);
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
typename Node<Link, T>::container_type& Node<Link, T>::links()
{
    return links_;
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
