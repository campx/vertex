#pragma once
#include <vertex/traversal.h>
#include <stack>

namespace vertex {
template <typename Container>
class post_order_traversal
    : public traversal<Container, post_order_traversal<Container>> {
 public:
  using base_type = traversal<Container, post_order_traversal<Container>>;
  using base_type::is_traversable;
  using base_type::position;
  using base_type::root;
  using base_type::vertices;
  using typename base_type::predicate_type;

  post_order_traversal(
      const Container& vertices, typename Container::const_iterator root,
      predicate_type predicate = typename base_type::unconditional_traversal());

  bool traverseRight();
  bool traverseLeft();
  bool next();

 private:
  std::stack<typename base_type::edge_type> to_visit_;
  typename base_type::vertex_iterator prev_pos_;
};

template <typename Container>
post_order_traversal<Container>::post_order_traversal(
    const Container& vertices, typename Container::const_iterator root,
    typename post_order_traversal<Container>::predicate_type predicate)
    : base_type(vertices, root, predicate), prev_pos_(vertices.end()) {
  if (position() != base_type::vertices().end()) {
    auto e = edge<Container>::root(position()->first);
    to_visit_.push(e);
    next();
  }
}

template <typename Container>
bool post_order_traversal<Container>::traverseLeft() {
  auto moved = false;
  while (position()->second.size() ==
         2) {  // traversal to bottom of left branch
    auto left_key = *position()->second.begin();
    auto right_child = vertices().find(*(++position()->second.begin()));
    auto left_child = vertices().find(left_key);
    auto left_edge = edge<Container>(position()->first, left_key);
    if (right_child == prev_pos_ || left_child == prev_pos_ ||
        left_child == vertices().end() || !is_traversable(left_edge)) {
      moved = false;
      break;
    } else {
      to_visit_.push(left_edge);
      base_type::position(left_child);
      moved = true;
    }
  }
  if (moved) {
    to_visit_.pop();
  }
  return moved;
}

template <typename Container>
bool post_order_traversal<Container>::traverseRight() {
  auto moved = false;
  if (position()->second.size() == 2) {  // traverse right branch
    auto child_key = *(++position()->second.begin());
    auto child_vertex = vertices().find(child_key);
    auto child_edge = edge<Container>(position()->first, child_key);
    if (child_vertex != vertices().end() && child_vertex != prev_pos_ &&
        is_traversable(child_edge)) {  // don't move to right child if null
      to_visit_.push(child_edge);
      base_type::position(child_vertex);
      moved = true;
    }
  }
  return moved;
}

template <typename Container>
bool post_order_traversal<Container>::next() {
  auto moved = false;
  prev_pos_ = position();
  while (!to_visit_.empty()) {
    if (!moved) {
      auto edge = to_visit_.top();
      base_type::position(vertices().find(edge.target()));
      moved = true;
    } else if (traverseLeft()) {
      moved = true;
      break;
    } else if (traverseRight()) {
      moved = true;
    } else if (!to_visit_.empty()) {
      to_visit_.pop();
      break;
    }
  }
  return moved;
}

}  // namespace vertex
