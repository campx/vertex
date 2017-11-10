#pragma once

#include <memory>
#include <queue>
#include <vertex/Traversal.h>

namespace vertex
{

/** Bredth first tree traversal */
template <typename VertexMap,
          typename Predicate = NullaryPredicate<VertexMap, true>>
class BreadthFirstTraversal
        : public Traversal<VertexMap,
                           BreadthFirstTraversal<VertexMap, Predicate>,
                           Predicate>
{
public:
    using self_type = BreadthFirstTraversal<VertexMap, Predicate>;
    using base_type = Traversal<VertexMap, self_type, Predicate>;
    using base_type::base_type;
    using base_type::vertices;
    using base_type::vertex;
    using base_type::position;
    using base_type::edge;
    using base_type::isTraversible;

    bool next();

private:
    std::queue<typename base_type::edge_type> to_visit_;
};

template <typename VertexMap, typename Predicate>
bool BreadthFirstTraversal<VertexMap, Predicate>::next()
{
    auto result = false;
    if (position() != vertices()->end())
    {
        for (const auto& child : position()->second)
        {
            auto e = std::make_pair(position()->first, child);
            if (isTraversible(e))
            {
                to_visit_.push(e);
            }
        }
    } else
    {
        result = false;
    }
    while (!to_visit_.empty())
    {
        base_type::edge(to_visit_.front());
        to_visit_.pop();
        base_type::position(vertices()->find(edge().second));
        if (position() != vertices()->end())
        {
            base_type::vertex(position()->second);
            result = true;
            break;
        }
    }
    return result;
}

} // namespace vertex
