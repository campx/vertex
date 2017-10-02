#pragma once
#include <stack>
#include <vertex/Traversal.h>

namespace vertex
{
/** TODO put logic currently in Iterator into the Traversal class */
template <typename VertexStore,
          typename Predicate = NullaryPredicate<VertexStore, true>>
class InOrderTraversal
    : public Traversal<VertexStore,
                       InOrderTraversal<VertexStore, Predicate>,
                       Predicate>
{
public:
    InOrderTraversal(VertexStore* vertices,
                     typename VertexStore::iterator root,
                     Predicate predicate = Predicate{});
    using base_type = Traversal<VertexStore,
                                InOrderTraversal<VertexStore, Predicate>,
                                Predicate>;
    using base_type::vertices;
    using base_type::vertex;
    using base_type::position;
    using base_type::root;
    using base_type::edge;
    using base_type::isTraversible;

    bool next();

private:
    std::stack<typename base_type::edge_type> to_visit_;
};

template <typename VertexStore, typename Predicate>
InOrderTraversal<VertexStore, Predicate>::InOrderTraversal(
    VertexStore* vertices,
    typename VertexStore::iterator root,
    Predicate predicate)
    : base_type(vertices, root, predicate)
{
    if (position() != base_type::vertices()->end())
    {
        to_visit_.push(edge());
        next();
    }
}

template <typename VertexStore, typename Predicate>
bool InOrderTraversal<VertexStore, Predicate>::next()
{
    if (to_visit_.empty())
    {
        return false;
    }
    auto child = position();
    while (child != vertices()->end() && child->second.size() == 2)
    { // traversal to bottom of left branch
        auto next_child = vertices()->find(*child->second.begin());
        auto e = std::make_pair(child->first, *child->second.begin());
        if (next_child != vertices()->end() && isTraversible(e))
        {
            to_visit_.push(e);
        }
        child = next_child;
    }
    base_type::edge(to_visit_.top());
    to_visit_.pop();
    child = vertices()->find(edge().second);
    if (child != vertices()->end())
    { // next child on stack is not null
        base_type::position(child);
    }
    base_type::vertex(position()->second);
    if (position()->second.size() == 2)
    { // traverse right branch
        auto link = *(++position()->second.begin());
        child = vertices()->find(link);
        auto e = std::make_pair(position()->first, link);
        if (child != vertices()->end() && isTraversible(e))
        { // don't move to right child if null
            to_visit_.push(e);
            base_type::position(child);
        }
    }
    return true;
}

} // namespace vertex
