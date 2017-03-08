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
                  container_type links = container_type())
        : data_(std::move(data)), links_(std::move(links))
    {
    }

    /** Get the data associated with this node */
    const element_type& data() const
    {
        return data_;
    }

    /** Get the hash of this node */
    value_type hash() const;

    const container_type& links() const
    {
        return links_;
    }

    bool operator==(const Node& rhs) const;

    bool operator!=(const Node& rhs) const;

    bool operator>(const Node& rhs) const;

    bool operator<(const Node& rhs) const;

private:
    element_type data_;
    container_type links_;
};

template <typename T, typename Link, typename Impl>
typename Node<T, Link, Impl>::value_type Node<T, Link, Impl>::hash() const
{
    return static_cast<const Impl*>(this)->computeHash();
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
