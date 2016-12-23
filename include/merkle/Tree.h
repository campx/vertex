#pragma once
#include <memory>
#include <merkle/Forest.h>

namespace merkle
{

template <typename Forest>
class Tree
{
public:
    using node_iterator = typename Forest::node_iterator;
    Tree(Forest* forest, node_iterator root) : forest_(forest), root_(root) {}

    node_iterator root() const { return root_; }

private:
    Forest* const forest_;
    node_iterator root_;
};

} // namespace merkle
