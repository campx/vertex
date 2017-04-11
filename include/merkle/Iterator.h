#pragma once
#include <memory>

namespace merkle
{

/** Iterates through all links of a node, dereferencing to a link-node pair */
template <typename NodeStore>
class NodeLinkIterator
{
public:
    using self_type = NodeLinkIterator<NodeStore>;

    using link_type = typename NodeStore::key_type;
    using mapped_type = typename NodeStore::mapped_type;
    using value_type = typename NodeStore::value_type;
    using reference = const value_type&;
    using pointer = const value_type*;

    using node_iterator = typename NodeStore::iterator;
    using link_container = typename mapped_type::container_type;
    using link_iterator = typename link_container::const_iterator;

    NodeLinkIterator(NodeStore nodes, link_iterator link_it);
    NodeLinkIterator(const NodeLinkIterator&) = default;
    NodeLinkIterator& operator=(const NodeLinkIterator&) = default;

    self_type operator++();
    self_type operator++(int dummy);
    self_type operator--();
    self_type operator--(int dummy);
    reference operator*();
    pointer operator->();

    bool operator==(const self_type& rhs) const;
    bool operator!=(const self_type& rhs) const;

private:
    NodeStore nodes_;
    link_iterator link_it_;
    node_iterator node_it_;
};

template <typename NodeStore>
NodeLinkIterator<NodeStore>::NodeLinkIterator(
    NodeStore nodes,
    typename NodeLinkIterator<NodeStore>::link_iterator link_it)
    : nodes_(std::move(nodes)), link_it_(std::move(link_it))
{
}

template <typename NodeStore>
typename NodeLinkIterator<NodeStore>::self_type NodeLinkIterator<NodeStore>::
operator++()
{
    ++link_it_;
    return *this;
}

template <typename NodeStore>
typename NodeLinkIterator<NodeStore>::self_type NodeLinkIterator<NodeStore>::
operator++(int dummy)
{
    dummy++;
    auto tmp = *this;
    ++link_it_;
    return tmp;
}

template <typename NodeStore>
typename NodeLinkIterator<NodeStore>::self_type NodeLinkIterator<NodeStore>::
operator--()
{
    --link_it_;
    return *this;
}

template <typename NodeStore>
typename NodeLinkIterator<NodeStore>::self_type NodeLinkIterator<NodeStore>::
operator--(int dummy)
{
    dummy--;
    auto tmp = *this;
    --link_it_;
    return tmp;
}

template <typename NodeStore>
typename NodeLinkIterator<NodeStore>::reference NodeLinkIterator<NodeStore>::
operator*()
{
    node_it_ = nodes_.find(*(link_it_));
    return node_it_.operator*();
}

template <typename NodeStore>
typename NodeLinkIterator<NodeStore>::pointer NodeLinkIterator<NodeStore>::
operator->()
{
    node_it_ = nodes_.find(*(link_it_));
    return node_it_.operator->();
}

template <typename NodeStore>
bool NodeLinkIterator<NodeStore>::operator==(const NodeLinkIterator& rhs) const
{
    return nodes_ == rhs.nodes_ && link_it_ == rhs.link_it_;
}

template <typename NodeStore>
bool NodeLinkIterator<NodeStore>::operator!=(const NodeLinkIterator& rhs) const
{
    return !(*this == rhs);
}

} // namespace merkle
