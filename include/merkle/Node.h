#pragma once
#include <algorithm>
#include <vector>

namespace merkle
{

template <typename T, typename Link, typename Impl>
class Node
{
public:
    /** Type of data stored by a Node */
    using element_type = T;
    /** Result type of hashing a Node */
    using value_type = Link;

    using container_type = std::vector<value_type>;

    explicit Node(element_type data = element_type(),
                  container_type links = container_type());

    /** Get the data associated with this node */
    const element_type& data() const;

    /** Set the data associated with this node */
    void data(const element_type& value);

    /** Get the hash of this node */
    value_type self_link() const;

    /** Get the nodes linked to by this node */
    const container_type& links() const;

    /** Set the nodes linked to by this node */
    void links(const container_type& value);

    bool operator==(const Node& rhs) const;

    bool operator!=(const Node& rhs) const;

    bool operator>(const Node& rhs) const;

    bool operator<(const Node& rhs) const;

private:
    element_type data_;
    container_type links_;
};

template <typename T, typename Link, typename Impl>
Node<T, Link, Impl>::Node(element_type data, container_type links)
    : data_(std::move(data)), links_(std::move(links))
{
}

template <typename T, typename Link, typename Impl>
const typename Node<T, Link, Impl>::element_type&
Node<T, Link, Impl>::data() const
{
    return data_;
}

template <typename T, typename Link, typename Impl>
void Node<T, Link, Impl>::data(
    const typename Node<T, Link, Impl>::element_type& value)
{
    data_ = value;
}

template <typename T, typename Link, typename Impl>
typename Node<T, Link, Impl>::value_type Node<T, Link, Impl>::self_link() const
{
    return static_cast<const Impl*>(this)->self_link();
}

template <typename T, typename Link, typename Impl>
const typename Node<T, Link, Impl>::container_type&
Node<T, Link, Impl>::links() const
{
    return links_;
}

template <typename T, typename Link, typename Impl>
void Node<T, Link, Impl>::links(
    const typename Node<T, Link, Impl>::container_type& value)
{
    links_ = value;
}

template <typename T, typename Link, typename Impl>
bool Node<T, Link, Impl>::operator==(const Node<T, Link, Impl>& rhs) const
{
    return self_link() == rhs.self_link();
}

template <typename T, typename Link, typename Impl>
bool Node<T, Link, Impl>::operator!=(const Node<T, Link, Impl>& rhs) const
{
    return self_link() != rhs.self_link();
}

template <typename T, typename Link, typename Impl>
bool Node<T, Link, Impl>::operator<(const Node<T, Link, Impl>& rhs) const
{
    return self_link() < rhs.self_link();
}

template <typename T, typename Link, typename Impl>
bool Node<T, Link, Impl>::operator>(const Node<T, Link, Impl>& rhs) const
{
    return self_link() > rhs.self_link();
}

} // namespace merkle
