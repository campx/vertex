#pragma once
#include <vertex/traversal.h>
#include <stack>

namespace vertex {
template <typename Container,
          typename Predicate = NullaryPredicate<Container, true>>
class post_order_traversal
    : public traversal<Container, post_order_traversal<Container, Predicate>,
                       Predicate> {
 public:
  using base_type =
      traversal<Container, post_order_traversal<Container, Predicate>,
                Predicate>;
  using base_type::isTraversible;
  using base_type::position;
  using base_type::root;
  using base_type::vertices;

  post_order_traversal(const Container& vertices,
                       typename Container::const_iterator root,
                     Predicate predicate = Predicate{});
  bool traverseRight();
  bool traverseLeft();
  bool next();

 private:
  std::stack<typename base_type::edge_type> to_visit_;
  typename base_type::vertex_iterator prev_pos_;
};

template <typename Container, typename Predicate>
post_order_traversal<Container, Predicate>::post_order_traversal(
    const Container& vertices, typename Container::const_iterator root,
    Predicate predicate)
    : base_type(vertices, root, predicate), prev_pos_(vertices.end()) {
  if (position() != base_type::vertices().end()) {
    using key_type = typename Container::key_type;
    auto edge = std::make_pair(key_type{}, position()->first);
    to_visit_.push(edge);
    next();
  }
}

template <typename Container, typename Predicate>
bool post_order_traversal<Container, Predicate>::traverseLeft() {
  auto moved = false;
  while (position()->second.links().size() ==
         2) {  // traversal to bottom of left branch
    auto left_key = *position()->second.links().begin();
    auto right_child = vertices().find(*(++position()->second.links().begin()));
    auto left_child = vertices().find(left_key);
    auto left_edge = std::make_pair(position()->first, left_key);
    if (right_child == prev_pos_ || left_child == prev_pos_ ||
        left_child == vertices().end() || !isTraversible(left_edge)) {
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

template <typename Container, typename Predicate>
bool post_order_traversal<Container, Predicate>::traverseRight() {
  auto moved = false;
  if (position()->second.links().size() == 2) {  // traverse right branch
    auto child_key = *(++position()->second.links().begin());
    auto child_vertex = vertices().find(child_key);
    auto child_edge = std::make_pair(position()->first, child_key);
    if (child_vertex != vertices().end() && child_vertex != prev_pos_ &&
        isTraversible(child_edge)) {  // don't move to right child if null
      to_visit_.push(child_edge);
      base_type::position(child_vertex);
      moved = true;
    }
  }
  return moved;
}

template <typename Container, typename Predicate>
bool post_order_traversal<Container, Predicate>::next() {
  auto moved = false;
  prev_pos_ = position();
  while (!to_visit_.empty()) {
    if (!moved) {
      auto edge = to_visit_.top();
      base_type::position(vertices().find(edge.second));
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
