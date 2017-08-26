#pragma once
#include <memory>
#include <queue>

namespace vertex
{

/** An Edge generator which traverses a VertexStore in bredth-first order */
template <typename VertexStore>
class BredthFirstTraversal
{
public:
    using vertex_store_type = VertexStore;
    using vertex_iterator = typename VertexStore::iterator;
    using value_type = std::pair<typename VertexStore::key_type,
                                 typename VertexStore::key_type>;
    BredthFirstTraversal(VertexStore* vertices,
                         typename VertexStore::iterator root);

    std::pair<value_type, bool> operator()();
    VertexStore* vertices() const;
    const vertex_iterator& root() const;
    const vertex_iterator& vertex() const;
    const value_type& edge() const;

private:
    VertexStore* vertices_;
    vertex_iterator root_;
    vertex_iterator vertex_;
    std::queue<value_type> to_visit_;
    value_type edge_;
};

template <typename VertexStore>
BredthFirstTraversal<VertexStore>::BredthFirstTraversal(
    VertexStore* vertices, typename VertexStore::iterator root)
    : vertices_(vertices), root_(std::move(root)), vertex_(root_)
{
}

template <typename VertexStore>
VertexStore* BredthFirstTraversal<VertexStore>::vertices() const
{
    return vertices_;
}

template <typename VertexStore>
const typename VertexStore::iterator&
BredthFirstTraversal<VertexStore>::root() const
{
    return root_;
}

template <typename VertexStore>
const typename VertexStore::iterator&
BredthFirstTraversal<VertexStore>::vertex() const
{
    return vertex_;
}

template <typename VertexStore>
const typename BredthFirstTraversal<VertexStore>::value_type&
BredthFirstTraversal<VertexStore>::edge() const
{
    return edge_;
}

template <typename VertexStore>
std::pair<typename BredthFirstTraversal<VertexStore>::value_type, bool>
BredthFirstTraversal<VertexStore>::operator()()
{
    auto result = std::make_pair(value_type{}, false);
    if (vertex() != vertices()->end())
    {
        for (const auto& child : vertex()->second)
        {
            to_visit_.push(std::make_pair(vertex()->first, child));
        }
    }
    if (!to_visit_.empty())
    {
        edge_ = to_visit_.front();
        result = std::make_pair(edge_, true);
        to_visit_.pop();
        vertex_ = vertices()->find(edge_.second);
    }
    return result;
}

} // namespace vertex
