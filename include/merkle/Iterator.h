#pragma once
#include <memory>

namespace merkle
{

/**
 * @todo Write a breadth-first iterator, which takes a predicate for
 * traversal, by default a function which always returns true */

/** Iterates through all links of a node, dereferencing to a link-node pair */
template <typename NodeStore>
class NodeLinkIterator : public std::iterator<std::input_iterator_tag,
                                              typename NodeStore::value_type>
{
public:
    using self_type = NodeLinkIterator<NodeStore>;

    using link_type = typename NodeStore::key_type;
    using mapped_type = typename NodeStore::mapped_type;
    using value_type = typename NodeStore::value_type;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::input_iterator_tag;

    using node_iterator = typename NodeStore::const_iterator;
    using link_container = typename mapped_type::container_type;
    using link_iterator = typename link_container::const_iterator;

    NodeLinkIterator(const NodeStore* nodes,
                     link_iterator link_it = link_iterator{});
    NodeLinkIterator(const NodeLinkIterator&) = default;
    NodeLinkIterator& operator=(const NodeLinkIterator&) = default;

    node_iterator node();

    self_type operator++();
    self_type operator++(int dummy);
    self_type operator--();
    self_type operator--(int dummy);
    reference operator*();
    pointer operator->();

    bool operator==(const self_type& rhs) const;
    bool operator!=(const self_type& rhs) const;

private:
    const NodeStore* nodes_;
    link_iterator link_it_;
};

template <typename NodeStore>
NodeLinkIterator<NodeStore>::NodeLinkIterator(
    const NodeStore* nodes,
    typename NodeLinkIterator<NodeStore>::link_iterator link_it)
    : nodes_(nodes), link_it_(std::move(link_it))
{
}

template <typename NodeStore>
typename NodeLinkIterator<NodeStore>::node_iterator
NodeLinkIterator<NodeStore>::node()
{
    // need to add an iterator to forest which will contain a pair of link,node
    // from find with the given link
    auto result = nodes_->find(*(link_it_));
    return result;
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
    return node().operator*();
}

template <typename NodeStore>
typename NodeLinkIterator<NodeStore>::pointer NodeLinkIterator<NodeStore>::
operator->()
{
    return node().operator->();
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
