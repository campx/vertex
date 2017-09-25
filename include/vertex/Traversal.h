#pragma once

namespace vertex
{

/** A predicate which always returns either true or false regardless of the
 * input */
template <typename VertexStore, bool Result>
struct NullaryPredicate
{
    using result_type = bool;
    using argument_type = std::pair<typename VertexStore::key_type,
                                    typename VertexStore::key_type>;
    bool operator()(const argument_type& value) const;
};

template <typename VertexStore, bool Result>
bool NullaryPredicate<VertexStore, Result>::
operator()(const typename NullaryPredicate<VertexStore, Result>::argument_type&
               value) const
{
    (void)value;
    return Result;
}

template <typename VertexStore,
          typename Impl,
          typename Predicate = NullaryPredicate<VertexStore, true>>
class Traversal
{
public:
    using vertex_store_type = VertexStore;
    using vertex_iterator = typename VertexStore::iterator;
    using vertex_type = typename VertexStore::mapped_type;
    using key_type = typename VertexStore::key_type;
    using edge_type = std::pair<key_type, key_type>;
    using predicate_type = Predicate;

    Traversal(VertexStore* vertices,
              typename VertexStore::iterator root,
              Predicate predicate = Predicate{});

    VertexStore* vertices() const;
    const vertex_iterator& root() const;
    const vertex_iterator& position() const;
    const vertex_type& vertex() const;
    const edge_type& edge() const;
    const predicate_type& predicate() const;
    bool isTraversible(const edge_type& value) const;

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
    Predicate predicate_;
};

template <typename VertexStore, typename Impl, typename Predicate>
Traversal<VertexStore, Impl, Predicate>::Traversal(
    VertexStore* vertices,
    typename VertexStore::iterator root,
    Predicate predicate)
    : vertices_(vertices), root_(std::move(root)), position_(root_),
      vertex_(position_ == vertices_->end() ? vertex_type{} :
                                              position_->second),
      edge_(std::make_pair(key_type{},
                           position_ == vertices_->end() ? key_type{} :
                                                           position_->first)),
      predicate_(std::move(predicate))
{
}

template <typename VertexStore, typename Impl, typename Predicate>
VertexStore* Traversal<VertexStore, Impl, Predicate>::vertices() const
{
    return vertices_;
}

template <typename VertexStore, typename Impl, typename Predicate>
const typename Traversal<VertexStore, Impl, Predicate>::vertex_iterator&
Traversal<VertexStore, Impl, Predicate>::root() const
{
    return root_;
}

template <typename VertexStore, typename Impl, typename Predicate>
const typename Traversal<VertexStore, Impl, Predicate>::vertex_iterator&
Traversal<VertexStore, Impl, Predicate>::position() const
{
    return position_;
}

template <typename VertexStore, typename Impl, typename Predicate>
const typename Traversal<VertexStore, Impl, Predicate>::vertex_type&
Traversal<VertexStore, Impl, Predicate>::vertex() const
{
    return vertex_;
}

template <typename VertexStore, typename Impl, typename Predicate>
const typename Traversal<VertexStore, Impl, Predicate>::edge_type&
Traversal<VertexStore, Impl, Predicate>::edge() const
{
    return edge_;
}

template <typename VertexStore, typename Impl, typename Predicate>
const typename Traversal<VertexStore, Impl, Predicate>::predicate_type&
Traversal<VertexStore, Impl, Predicate>::predicate() const
{
    return predicate_;
}

template <typename VertexStore, typename Impl, typename Predicate>
void Traversal<VertexStore, Impl, Predicate>::vertex(
    const typename Traversal<VertexStore, Impl, Predicate>::vertex_type& value)
{
    vertex_ = value;
}

template <typename VertexStore, typename Impl, typename Predicate>
void Traversal<VertexStore, Impl, Predicate>::position(
    const typename Traversal<VertexStore, Impl, Predicate>::vertex_iterator&
        value)
{
    position_ = value;
}

template <typename VertexStore, typename Impl, typename Predicate>
void Traversal<VertexStore, Impl, Predicate>::edge(
    const typename Traversal<VertexStore, Impl, Predicate>::edge_type& value)
{
    edge_ = value;
}

template <typename VertexStore, typename Impl, typename Predicate>
bool Traversal<VertexStore, Impl, Predicate>::isTraversible(
    const typename Traversal<VertexStore, Impl, Predicate>::edge_type& value)
    const
{
    return predicate_(value);
}

} // namespace vertex
