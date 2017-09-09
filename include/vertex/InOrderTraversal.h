#pragma once
#include <stack>
#include <vertex/Traversal.h>

namespace vertex
{
template <typename VertexStore>
class InOrderTraversal
    : public Traversal<VertexStore, InOrderTraversal<VertexStore>>
{
public:
    InOrderTraversal(VertexStore* vertices,
                     typename VertexStore::iterator root);
    using base_type = Traversal<VertexStore, InOrderTraversal<VertexStore>>;
    using base_type::vertices;
    using base_type::vertex;
    using base_type::position;
    using base_type::root;
    using base_type::edge;

    bool advance();

private:
    std::stack<typename base_type::edge_type> to_visit_;
    typename base_type::edge_type prev_edge_;
};

template <typename VertexStore>
InOrderTraversal<VertexStore>::InOrderTraversal(
    VertexStore* vertices, typename VertexStore::iterator root)
    : base_type(vertices, root)
{
    if (position() != base_type::vertices()->end())
    {
        to_visit_.push(edge());
        advance();
    }
}

template <typename VertexStore>
bool InOrderTraversal<VertexStore>::advance()
{
    auto result = true;
    auto child = position();
    while (child != vertices()->end() && child->second.size() == 2)
    { // traversal to bottom of left branch
        auto e = std::make_pair(child->first, *child->second.begin());
        to_visit_.push(e);
        child = vertices()->find(*child->second.begin());
    }
    if (!to_visit_.empty())
    {
        prev_edge_ = edge();
        base_type::edge(to_visit_.top());
        base_type::position(vertices()->find(edge().second));
        base_type::vertex(position()->second);
        to_visit_.pop();
        if (position()->second.size() == 2)
        { // traverse right branch
            auto child = *(++position()->second.begin());
            to_visit_.push(std::make_pair(position()->first, child));
            base_type::position(vertices()->find(child));
        }
    }
    else
    {
        result = false;
    }
    return result;
}

} // namespace vertex
