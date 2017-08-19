#pragma once
#include <memory>
#include <vertex/Iterator.h>

namespace vertex
{

template <typename Tree>
class Array
{
public:
    using vertex_iterator = typename Tree::vertex_iterator;
    using vertex_type = typename Tree::vertex_type;
    using vertex_container = typename Tree::vertex_container;
    using iterator = VertexEdgeIterator<vertex_container>;
    explicit Array(Tree tree = Tree());
    void root(vertex_iterator vertex);
    vertex_iterator root() const;
    bool empty() const;
    iterator begin() const;
    iterator end() const;
    iterator insert(const vertex_type&);

private:
    Tree tree_;
};

template <typename Tree>
Array<Tree>::Array(Tree tree) : tree_(std::move(tree))
{
}

template <typename Tree>
void Array<Tree>::root(typename Array<Tree>::vertex_iterator vertex)
{
    tree_.root(vertex);
}

template <typename Tree>
typename Array<Tree>::vertex_iterator Array<Tree>::root() const
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
    return iterator(&tree_.vertices(), tree_.root()->second.edges().begin());
}

template <typename Tree>
typename Array<Tree>::iterator Array<Tree>::end() const
{
    return iterator(&tree_.vertices(), tree_.root()->second.edges().end());
}

template <typename Tree>
typename Array<Tree>::iterator
Array<Tree>::insert(const typename Array<Tree>::vertex_type& vertex)
{
    vertex.self_edge();
    auto inserted = tree_.insert(tree_.root(), vertex);
    auto element = inserted->second.edges().begin();
    return iterator(&tree_.vertices(), element);
}

} // namespace vertex
