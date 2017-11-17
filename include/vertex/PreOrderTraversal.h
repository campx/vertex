#pragma once
#include <stack>
#include <vertex/Traversal.h>

namespace vertex
{
template <typename VertexMap,
          typename Predicate = NullaryPredicate<VertexMap, true>>
class PreOrderTraversal
    : public Traversal<VertexMap,
                       PreOrderTraversal<VertexMap, Predicate>,
                       Predicate>
{
public:
    using base_type = Traversal<VertexMap,
                                PreOrderTraversal<VertexMap, Predicate>,
                                Predicate>;
    using base_type::vertices;
    using base_type::vertex;
    using base_type::position;
    using base_type::root;
    using base_type::edge;
    using base_type::isTraversible;

    PreOrderTraversal(VertexMap* vertices,
                      typename base_type::vertex_iterator root,
                      Predicate predicate = Predicate{});
    bool traverseRight();
    bool traverseLeft();
    bool next();

private:
    std::stack<typename base_type::edge_type> to_visit_;
    typename base_type::vertex_iterator prev_pos_;
};

template <typename VertexMap, typename Predicate>
PreOrderTraversal<VertexMap, Predicate>::PreOrderTraversal(
    VertexMap* vertices,
    typename base_type::vertex_iterator root,
    Predicate predicate)
    : base_type(vertices, root, predicate), prev_pos_(vertices->end())
{
    if (position() != base_type::vertices()->end())
    {
        to_visit_.push(edge());
    }
}

template <typename VertexMap, typename Predicate>
bool PreOrderTraversal<VertexMap, Predicate>::traverseLeft()
{
    auto moved = false;
    if (position()->second.size() == 2)
    { // traversal to bottom of left branch
        auto left_key = *position()->second.begin();
        auto right_child = vertices()->find(*(++position()->second.begin()));
        auto left_child = vertices()->find(left_key);
        auto left_edge = std::make_pair(position()->first, left_key);
        if (left_child == prev_pos_ || right_child == prev_pos_ ||
            left_child == vertices()->end() || !isTraversible(left_edge))
        {
            moved = false;
        }
        else
        {
            base_type::edge(left_edge);
            base_type::position(left_child);
            moved = true;
        }
    }
    return moved;
}

template <typename VertexMap, typename Predicate>
bool PreOrderTraversal<VertexMap, Predicate>::traverseRight()
{
    auto moved = false;
    if (position()->second.size() == 2)
    { // traverse right branch
        auto child_key = *(++position()->second.begin());
        auto child_vertex = vertices()->find(child_key);
        auto child_edge = std::make_pair(position()->first, child_key);
        if (child_vertex != prev_pos_ && child_vertex != vertices()->end() &&
            isTraversible(child_edge))
        {
            base_type::edge(child_edge);
            base_type::position(child_vertex);
            moved = true;
        }
    }
    return moved;
}

template <typename VertexMap, typename Predicate>
bool PreOrderTraversal<VertexMap, Predicate>::next()
{
    auto moved = false;
    prev_pos_ = position();
    while (!to_visit_.empty())
    {
        if (traverseLeft())
        {
            to_visit_.push(edge());
            moved = true;
            break;
        }
        else if (traverseRight())
        {
            to_visit_.push(edge());
            moved = true;
            break;
        }
        else
        {
            to_visit_.pop();
            prev_pos_ = position();
            if (!to_visit_.empty())
            {
                base_type::edge(to_visit_.top());
                base_type::position(vertices()->find(edge().second));
            }
        }
    }
    base_type::vertex(position()->second);
    return moved;
}

} // namespace vertex
