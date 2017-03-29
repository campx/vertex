#pragma once
#include <memory>
#include <merkle/Tree.h>

namespace merkle
{

/** Iterates through all children of a node */
template <typename Tree>
class SequentialIterator
{
public:
    using key_type = typename Tree::forest_type::key_type;
    using mapped_type = typename Tree::node_type;
    using self_type = SequentialIterator<Tree>;
    using node_iterator = typename Tree::node_iterator;
    using value_type = typename std::pair<const key_type, mapped_type>;
    using key_container = typename mapped_type::container_type;
    using key_iterator = typename key_container::const_iterator;
    using reference = const value_type&;
    using pointer = const value_type*;

    SequentialIterator(Tree tree, key_iterator node);
    SequentialIterator(const SequentialIterator&) = default;
    SequentialIterator& operator=(const SequentialIterator&) = default;

    self_type operator++();
    self_type operator++(int dummy);
    self_type operator--();
    self_type operator--(int dummy);
    reference operator*();
    pointer operator->();

    bool operator==(const self_type& rhs) const;
    bool operator!=(const self_type& rhs) const;

private:
    Tree tree_;
    key_iterator key_;
    node_iterator node_;
};

template <typename Tree>
SequentialIterator<Tree>::SequentialIterator(
    Tree tree, typename SequentialIterator<Tree>::key_iterator node)
    : tree_(std::move(tree)), key_(node)
{
    if (key_ != tree_.root()->second.links().end())
    {
        node_ = tree_.nodes().find(*key_);
    }
}

template <typename Tree>
typename SequentialIterator<Tree>::self_type SequentialIterator<Tree>::
operator++()
{
    ++key_;
    return *this;
}

template <typename Tree>
typename SequentialIterator<Tree>::self_type SequentialIterator<Tree>::
operator++(int dummy)
{
    dummy++;
    auto tmp = *this;
    ++key_;
    return tmp;
}

template <typename Tree>
typename SequentialIterator<Tree>::self_type SequentialIterator<Tree>::
operator--()
{
    --key_;
    return *this;
}

template <typename Tree>
typename SequentialIterator<Tree>::self_type SequentialIterator<Tree>::
operator--(int dummy)
{
    dummy--;
    auto tmp = *this;
    --key_;
    return tmp;
}

template <typename Tree>
typename SequentialIterator<Tree>::reference SequentialIterator<Tree>::
operator*()
{
    node_ = tree_.nodes().find(*(key_));
    return node_.operator*();
}

template <typename Tree>
typename SequentialIterator<Tree>::pointer SequentialIterator<Tree>::
operator->()
{
    node_ = tree_.nodes().find(*(key_));
    return node_.operator->();
}

template <typename Tree>
bool SequentialIterator<Tree>::operator==(const SequentialIterator& rhs) const
{
    return tree_ == rhs.tree_ && key_ == rhs.key_;
}

template <typename Tree>
bool SequentialIterator<Tree>::operator!=(const SequentialIterator& rhs) const
{
    return !(*this == rhs);
}

} // namespace merkle
