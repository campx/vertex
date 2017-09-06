#pragma once
#include <memory>
#include <queue>
#include <vertex/Traversal.h>

namespace vertex
{

/** Bredth first tree traversal */
template <typename VertexStore>
class BredthFirstTraversal
    : public Traversal<VertexStore, BredthFirstTraversal<VertexStore>>
{
public:
    using base_type =
        Traversal<VertexStore, BredthFirstTraversal<VertexStore>>;
    using base_type::base_type;
    using base_type::vertices;
    using base_type::vertex;
    using base_type::position;
    using base_type::edge;

    bool advance();

private:
    std::queue<typename base_type::edge_type> to_visit_;
};

template <typename VertexStore>
bool BredthFirstTraversal<VertexStore>::advance()
{
    auto result = false;
    if (position() != vertices()->end())
    {
        for (const auto& child : position()->second)
        {
            to_visit_.push(std::make_pair(position()->first, child));
        }
    }
    if (!to_visit_.empty())
    {
        base_type::edge(to_visit_.front());
        to_visit_.pop();
        base_type::position(vertices()->find(edge().second));
        if (position() != vertices()->end())
        {
            base_type::vertex(position()->second);
        }
        result = true;
    }
    return result;
}

} // namespace vertex
