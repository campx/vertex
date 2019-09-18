#pragma once

#include <type_traits>

namespace vertex {

template <typename Container>
class edge {
 public:
  using vertex_type = typename Container::mapped_type;
  using vertex_iterator = typename Container::iterator;
  using link_type = typename vertex_type::link_type;
  using link_iterator = typename vertex_type::iterator;

  edge();

  /** Create an Edge from two links */
  edge(Container* vertices, const link_type& source, const link_type& target);

  /** Create an Edge from Node and Link iterators */
  edge(vertex_iterator v, link_iterator l);

  /** Determine whether the edge links to a root Node
   * A root edge has no source */
  [[nodiscard]] bool root() const;

  /** Determine whether the edge links to a leaf Node
   * A leaf edge has no target */
  [[nodiscard]] bool leaf() const;

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
edge<Container>::edge() : vertices_(nullptr) {}

template <typename Container>
edge<Container>::edge(Container* vertices,
                      const typename edge<Container>::link_type& source,
                      const typename edge<Container>::link_type& target)
    : vertices_(vertices),
      source_(vertices_ ? vertices_->find(source)
                        : edge<Container>::link_type()),
      target_(vertices_ ? vertices_->find(target)
                        : edge<Container>::link_type()),
      link_((vertices_ && source_ != vertices_->end())
                ? source_->find(target)
                : edge<Container>::link_iterator()) {}

}  // namespace vertex
