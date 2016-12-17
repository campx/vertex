#pragma once
#include <memory>
#include <merkle/Graph.h>

namespace merkle
{

template <typename NodeStore, typename LinkStore, typename Impl>
class Tree
{
public:
    using graph_type = typename Graph<NodeStore, LinkStore>;
    using key_type = graph_type::key_type;
    Tree(graph_type* graph, graph_type::key_type root)
        : graph_(graph), root_(std::move(root))
    {
    }

    // find method returns a vector of key_type, which constitutes the path
    // from root to the given key

private:
    graph_type* const graph_;
};

} // namespace merkle
