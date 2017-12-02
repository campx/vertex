#pragma once

#include <type_traits>

namespace vertex
{

template <typename Container>
class Edge
{
public:
    using vertex_type = typename Container::mapped_type;
    using vertex_iterator = typename Container::iterator;
    using link_type = typename vertex_type::link_type;
    using link_iterator = typename vertex_type::iterator;

    Edge();

    /** Create an Edge from two links */
    Edge(Container* vertices, const link_type& source, const link_type& target);

    /** Create an Edge from Node and Link iterators */
    Edge(vertex_iterator v, link_iterator l);

    /** Determine whether the edge links to a root Node
     * A root edge has no source */
    bool root() const;

    /** Determine whether the edge links to a leaf Node
     * A leaf edge has no target */
    bool leaf() const;

    /** Get the source Node */
    vertex_iterator source() const;

    /** Get the target Node */
    vertex_iterator target() const;

private:
    Container* vertices_;
    vertex_iterator source_;
    vertex_iterator target_;
    link_iterator link_;
};

template <typename Container>
Edge<Container>::Edge() : vertices_(nullptr)
{
}

template <typename Container>
Edge<Container>::Edge(Container* vertices, const typename
Edge<Container>::link_type& source,
                      const typename Edge<Container>::link_type& target)
        : vertices_(vertices), source_(vertices_ ? vertices_->find(source) :
                                       Edge<Container>::link_type()),
          target_(vertices_ ? vertices_->find(target)
                           : Edge<Container>::link_type()),
          link_((vertices_ && source_ != vertices_->end()) ? source_->find(
                  target) : Edge<Container>::link_iterator())
{
}

}
