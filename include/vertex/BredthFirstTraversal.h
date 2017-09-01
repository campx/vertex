#pragma once
#include <memory>
#include <queue>

namespace vertex
{

/** Bredth first tree traversal */
template <typename VertexStore>
class BredthFirstTraversal
{
public:
    using vertex_store_type = VertexStore;
    using vertex_iterator = typename VertexStore::iterator;
    using vertex_type = typename VertexStore::mapped_type;
    using key_type = typename VertexStore::key_type;
    using edge_type = std::pair<key_type, key_type>;

    BredthFirstTraversal(VertexStore* vertices,
                         typename VertexStore::iterator root);

    bool step();

    VertexStore* vertices() const;
    const vertex_iterator& root() const;
    const vertex_iterator& position() const;
    const vertex_type& vertex() const;
    const edge_type& edge() const;

private:
    VertexStore* vertices_;
    vertex_iterator root_;
    vertex_iterator it_;
    vertex_type vertex_;
    std::queue<edge_type> to_visit_;
    edge_type edge_;
};

template <typename VertexStore>
BredthFirstTraversal<VertexStore>::BredthFirstTraversal(
    VertexStore* vertices, typename VertexStore::iterator root)
    : vertices_(vertices), root_(std::move(root)), it_(root_),
      vertex_(it_ == vertices_->end() ? vertex_type{} : it_->second),
      edge_(std::make_pair(key_type{},
                           it_ == vertices_->end() ? key_type{} : it_->first))
{
}

template <typename VertexStore>
bool BredthFirstTraversal<VertexStore>::step()
{
    auto result = false;
    if (it_ != vertices()->end())
    {
        for (const auto& child : it_->second)
        {
            to_visit_.push(std::make_pair(it_->first, child));
        }
    }
    if (!to_visit_.empty())
    {
        edge_ = to_visit_.front();
        to_visit_.pop();
        it_ = vertices()->find(edge_.second);
        if (it_ != vertices()->end())
        {
            vertex_ = it_->second;
        }
        result = true;
    }
    return result;
}

template <typename VertexStore>
VertexStore* BredthFirstTraversal<VertexStore>::vertices() const
{
    return vertices_;
}

template <typename VertexStore>
const typename BredthFirstTraversal<VertexStore>::vertex_iterator&
BredthFirstTraversal<VertexStore>::root() const
{
    return root_;
}

template <typename VertexStore>
const typename BredthFirstTraversal<VertexStore>::vertex_iterator&
BredthFirstTraversal<VertexStore>::position() const
{
    return it_;
}

template <typename VertexStore>
const typename BredthFirstTraversal<VertexStore>::vertex_type&
BredthFirstTraversal<VertexStore>::vertex() const
{
    return vertex_;
}

template <typename VertexStore>
const typename BredthFirstTraversal<VertexStore>::edge_type&
BredthFirstTraversal<VertexStore>::edge() const
{
    return edge_;
}

} // namespace vertex
