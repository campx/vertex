#pragma once

#include <stack>
#include <algorithm>
#include <vertex/Traversal.h>

namespace vertex
{
template <typename Container,
          typename Predicate = NullaryPredicate<Container, true>>
class PreOrderTraversal : public Traversal<Container,
                                           PreOrderTraversal<Container,
                                                             Predicate>,
                                           Predicate>
{
public:
    using base_type = Traversal<Container,
                                PreOrderTraversal<Container, Predicate>,
                                Predicate>;
    using base_type::vertices;
    using base_type::position;
    using base_type::root;
    using base_type::isTraversible;

    PreOrderTraversal(const Container& vertices,
                      typename Container::const_iterator root,
                      Predicate predicate = Predicate{});

    bool next();

private:
    using vertex_type = typename Container::mapped_type;
    using link_type = typename vertex_type::container_type::value_type;

    bool traverse(link_type link);

    std::stack<typename base_type::edge_type> to_visit_;
    typename base_type::vertex_iterator prev_pos_;
};

template <typename Container, typename Predicate>
PreOrderTraversal<Container, Predicate>::PreOrderTraversal(
    const Container& vertices, typename Container::const_iterator root,
    Predicate predicate)
    : base_type(vertices, root, predicate), prev_pos_(vertices.end())
{
    if (position() != base_type::vertices().end())
    {
        using key_type = typename Container::key_type;
        auto edge = std::make_pair(key_type{}, position()->first);
        to_visit_.push(edge);
    }
}

template <typename Container, typename Predicate>
bool PreOrderTraversal<Container, Predicate>::traverse(link_type link)
{
    auto& links = position()->second.links();
    auto moved = false;
    auto child = vertices().find(link);
    auto edge = std::make_pair(position()->first, link);
    auto child_it = std::find(links.begin(), links.end(), prev_pos_->first);
    auto link_it = std::find(links.begin(), links.end(), link);
    if ((child_it == links.end() || link_it > child_it) &&
        child != vertices().end() && isTraversible(edge))
    {
        base_type::position(child);
        moved = true;
    }
    return moved;
};

template <typename Container, typename Predicate>
bool PreOrderTraversal<Container, Predicate>::next()
{
    auto moved = false;
    prev_pos_ = position();
    while (!to_visit_.empty())
    {
        auto source = position()->first;
        for (const auto& link : position()->second.links())
        {
            if (traverse(link))
            {
                moved = true;
                break;
            }
        }
        if (!moved)
        {
            to_visit_.pop();
            prev_pos_ = position();
            if (!to_visit_.empty())
            {
                auto edge = to_visit_.top();
                base_type::position(vertices().find(edge.second));
            }
        }
        else
        {
            auto edge = std::make_pair(source, position()->first);
            to_visit_.push(edge);
            break;
        }
    }
    return moved;
}

} // namespace vertex
