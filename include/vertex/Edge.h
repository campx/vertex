#pragma once

#include <type_traits>

namespace vertex
{

template <typename VertexMap>
class Edge
{
public:
    using vertex_type = typename VertexMap::mapped_type;
    using vertex_iterator = typename VertexMap::iterator;
    using link_type = typename vertex_type::link_type;
    using link_iterator = typename vertex_type::iterator;

    Edge();

    /** Create an Edge from two links */
    Edge(VertexMap* vertices, const link_type& source, const link_type& target);

    /** Create an Edge from Vertex and Link iterators */
    Edge(vertex_iterator v, link_iterator l);

    /** Determine whether the edge links to a root Vertex
     * A root edge has no source */
    bool root() const;

    /** Determine whether the edge links to a leaf Vertex
     * A leaf edge has no target */
    bool leaf() const;

    /** Get the source Vertex */
    vertex_iterator source() const;

    /** Get the target Vertex */
    vertex_iterator target() const;

private:
    VertexMap* vertices_;
    vertex_iterator source_;
    vertex_iterator target_;
    link_iterator link_;
};

template <typename VertexMap>
Edge<VertexMap>::Edge() : vertices_(nullptr)
{
}

template <typename VertexMap>
Edge<VertexMap>::Edge(VertexMap* vertices, const typename
Edge<VertexMap>::link_type& source,
                      const typename Edge<VertexMap>::link_type& target)
        : vertices_(vertices), source_(vertices_ ? vertices_->find(source) :
                                       Edge<VertexMap>::link_type()),
          target_(vertices_ ? vertices_->find(target)
                           : Edge<VertexMap>::link_type()),
          link_((vertices_ && source_ != vertices_->end()) ? source_->find(
                  target) : Edge<VertexMap>::link_iterator())
{
}

}
