#pragma once
#include <memory>

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
    using key_iterator = typename mapped_type::const_iterator;
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
class Array
{
public:
    using node_iterator = typename Tree::node_iterator;
    using node_type = typename Tree::node_type;
    using iterator = SequentialIterator<Tree>;
    explicit Array(Tree tree = Tree());
    node_iterator root() const;
    iterator begin();
    iterator end();
    iterator insert(const node_type&);

private:
    Tree tree_;
};

template <typename Tree>
Array<Tree>::Array(Tree tree) : tree_(std::move(tree))
{
}

template <typename Tree>
typename Array<Tree>::node_iterator Array<Tree>::root() const
{
    return tree_.root();
}

template <typename Tree>
typename Array<Tree>::iterator Array<Tree>::begin()
{
    return iterator(tree_, tree_.root()->second.begin());
}

template <typename Tree>
typename Array<Tree>::iterator Array<Tree>::end()
{
    return iterator(tree_, tree_.root()->second.end());
}

template <typename Tree>
typename Array<Tree>::iterator
Array<Tree>::insert(const typename Array<Tree>::node_type& node)
{
    node.hash();
    auto inserted = tree_.insert(tree_.root(), node);
    auto element = inserted->second.begin();
    return iterator(tree_, element);
}

template <typename Tree>
SequentialIterator<Tree>::SequentialIterator(
    Tree tree, typename SequentialIterator<Tree>::key_iterator node)
    : tree_(std::move(tree)), key_(node)
{
    if (key_ != tree_.root()->second.end())
    {
        node_ = tree_.forest()->nodes().find(*key_);
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
    node_ = tree_.forest()->nodes().find(*(key_));
    return node_.operator*();
}

template <typename Tree>
typename SequentialIterator<Tree>::pointer SequentialIterator<Tree>::
operator->()
{
    node_ = tree_.forest()->nodes().find(*(key_));
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
