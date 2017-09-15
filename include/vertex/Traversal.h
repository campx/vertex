#pragma once

namespace vertex
{

template <typename VertexStore, typename Impl>
class Traversal
{
public:
    using vertex_store_type = VertexStore;
    using vertex_iterator = typename VertexStore::iterator;
    using vertex_type = typename VertexStore::mapped_type;
    using key_type = typename VertexStore::key_type;
    using edge_type = std::pair<key_type, key_type>;

    Traversal(VertexStore* vertices, typename VertexStore::iterator root);

    VertexStore* vertices() const;
    const vertex_iterator& root() const;
    const vertex_iterator& position() const;
    const vertex_type& vertex() const;
    const edge_type& edge() const;

protected:
    void position(const vertex_iterator& value);
    void vertex(const vertex_type& value);
    void edge(const edge_type& value);

private:
    VertexStore* vertices_;
    vertex_iterator root_;
    vertex_iterator position_;
    vertex_type vertex_;
    edge_type edge_;
};

template <typename VertexStore, typename Impl>
Traversal<VertexStore, Impl>::Traversal(VertexStore* vertices,
                                        typename VertexStore::iterator root)
    : vertices_(vertices), root_(std::move(root)), position_(root_),
      vertex_(position_ == vertices_->end() ? vertex_type{} :
                                              position_->second),
      edge_(std::make_pair(key_type{},
                           position_ == vertices_->end() ? key_type{} :
                                                           position_->first))
{
}

template <typename VertexStore, typename Impl>
VertexStore* Traversal<VertexStore, Impl>::vertices() const
{
    return vertices_;
}

template <typename VertexStore, typename Impl>
const typename Traversal<VertexStore, Impl>::vertex_iterator&
Traversal<VertexStore, Impl>::root() const
{
    return root_;
}

template <typename VertexStore, typename Impl>
const typename Traversal<VertexStore, Impl>::vertex_iterator&
Traversal<VertexStore, Impl>::position() const
{
    return position_;
}

template <typename VertexStore, typename Impl>
const typename Traversal<VertexStore, Impl>::vertex_type&
Traversal<VertexStore, Impl>::vertex() const
{
    return vertex_;
}

template <typename VertexStore, typename Impl>
const typename Traversal<VertexStore, Impl>::edge_type&
Traversal<VertexStore, Impl>::edge() const
{
    return edge_;
}

template <typename VertexStore, typename Impl>
void Traversal<VertexStore, Impl>::vertex(
    const typename Traversal<VertexStore, Impl>::vertex_type& value)
{
    vertex_ = value;
}

template <typename VertexStore, typename Impl>
void Traversal<VertexStore, Impl>::position(
    const typename Traversal<VertexStore, Impl>::vertex_iterator& value)
{
    position_ = value;
}

template <typename VertexStore, typename Impl>
void Traversal<VertexStore, Impl>::edge(
    const typename Traversal<VertexStore, Impl>::edge_type& value)
{
    edge_ = value;
}

} // namespace vertex
