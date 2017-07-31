#pragma once
#include <memory>
#include <merkle/Iterator.h>

namespace merkle
{

template <typename Tree>
class Array
{
public:
    using node_iterator = typename Tree::node_iterator;
    using node_type = typename Tree::node_type;
    using node_container = typename Tree::node_container;
    using iterator = NodeLinkIterator<node_container>;
    explicit Array(Tree tree = Tree());
    void root(node_iterator node);
    node_iterator root() const;
    bool empty() const;
    iterator begin() const;
    iterator end() const;
    iterator insert(const node_type&);

private:
    Tree tree_;
};

template <typename Tree>
Array<Tree>::Array(Tree tree) : tree_(std::move(tree))
{
}

template <typename Tree>
void Array<Tree>::root(typename Array<Tree>::node_iterator node)
{
    tree_.root(node);
}

template <typename Tree>
typename Array<Tree>::node_iterator Array<Tree>::root() const
{
    return tree_.root();
}

template <typename Tree>
bool Array<Tree>::empty() const
{
    return begin() == end();
}

template <typename Tree>
typename Array<Tree>::iterator Array<Tree>::begin() const
{
    return iterator(&tree_.nodes(), tree_.root()->second.links().begin());
}

template <typename Tree>
typename Array<Tree>::iterator Array<Tree>::end() const
{
    return iterator(&tree_.nodes(), tree_.root()->second.links().end());
}

template <typename Tree>
typename Array<Tree>::iterator
Array<Tree>::insert(const typename Array<Tree>::node_type& node)
{
    node.self_link();
    auto inserted = tree_.insert(tree_.root(), node);
    auto element = inserted->second.links().begin();
    return iterator(&tree_.nodes(), element);
}

} // namespace merkle
