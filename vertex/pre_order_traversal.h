#pragma once

#include <vertex/traversal.h>
#include <algorithm>
#include <stack>

namespace vertex {
template <typename Container>
class pre_order_traversal
    : public traversal<Container, pre_order_traversal<Container>> {
 public:
  using base_type = traversal<Container, pre_order_traversal<Container>>;
  using base_type::is_traversable;
  using base_type::position;
  using base_type::root;
  using base_type::vertices;
  using typename base_type::predicate_type;

  pre_order_traversal();

  pre_order_traversal(
      const Container& vertices, typename Container::const_iterator root,
      predicate_type predicate = typename base_type::unconditional_traversal());

  bool next();

 private:
  using vertex_type = typename Container::mapped_type;
  using link_type = typename vertex_type::container_type::value_type;

  bool traverse(link_type link);

  std::stack<typename base_type::edge_type> to_visit_;
  typename base_type::vertex_iterator prev_pos_;
};

template <typename Container>
pre_order_traversal<Container>::pre_order_traversal() {}

template <typename Container>
pre_order_traversal<Container>::pre_order_traversal(
    const Container& vertices, typename Container::const_iterator root,
    predicate_type predicate)
    : base_type(vertices, root, predicate), prev_pos_(vertices.end()) {
  if (position() != base_type::vertices().end()) {
    auto e = edge<Container>::root(position()->first);
    to_visit_.push(e);
  }
}

template <typename Container>
bool pre_order_traversal<Container>::traverse(link_type link) {
  auto& node = position()->second;
  auto moved = false;
  auto child = vertices().find(link);
  auto e = edge<Container>(position()->first, link);
  auto child_it = node.find(prev_pos_->first);
  auto link_it = node.find(link);
  if ((child_it == node.end() || link_it > child_it) &&
      child != vertices().end() && is_traversable(e)) {
    base_type::position(child);
    moved = true;
  }
  return moved;
}

template <typename Container>
bool pre_order_traversal<Container>::next() {
  auto moved = false;
  prev_pos_ = position();
  while (!to_visit_.empty()) {
    auto source = position();
    for (const auto& link : position()->second) {
      if (traverse(link)) {
        moved = true;
        break;
      }
    }
    if (!moved) {
      to_visit_.pop();
      prev_pos_ = position();
      if (!to_visit_.empty()) {
        auto edge = to_visit_.top();
        base_type::position(vertices().find(edge.target()));
      }
    } else {
      auto e = edge<Container>(source->first, position()->first);
      to_visit_.push(e);
      break;
    }
  }
  return moved;
}

}  // namespace vertex
