#pragma once
#include <vertex/traversal.h>
#include <algorithm>
#include <stack>

namespace vertex {
template <typename Container>
class in_order_traversal
    : public traversal<Container, in_order_traversal<Container>> {
 public:
  using base_type = traversal<Container, in_order_traversal<Container>>;
  using base_type::is_traversable;
  using base_type::position;
  using base_type::root;
  using base_type::vertices;
  using typename base_type::predicate_type;

  in_order_traversal(
      const Container& vertices, typename Container::const_iterator root,
      predicate_type predicate = typename base_type::unconditional_traversal());
  bool next();

 private:
  std::stack<typename base_type::edge_type> to_visit_;
  typename base_type::vertex_iterator next_position_;
};

template <typename Container>
in_order_traversal<Container>::in_order_traversal(
    const Container& vertices, typename Container::const_iterator root,
    typename in_order_traversal<Container>::predicate_type predicate)
    : base_type(vertices, root, predicate), next_position_(position()) {
  if (position() != base_type::vertices().end()) {
    auto e = edge<Container>::root(position()->first);
    to_visit_.push(e);
    next();
  }
}

template <typename Container>
bool in_order_traversal<Container>::next() {
  if (to_visit_.empty()) {
    return false;
  }
  auto child = next_position_;
  while (child != vertices().end() &&
         child->second.links().size() >
             0) {  // traversal to bottom of left branch
    auto next_child = vertices().find(*child->second.links().begin());
    auto e = edge<Container>(child->first, next_child->first);
    if (next_child != vertices().end() && is_traversable(e)) {
      to_visit_.push(e);
    }
    child = next_child;
  }
  auto e = to_visit_.top();
  to_visit_.pop();
  child = vertices().find(e.target());
  if (child != vertices().end()) {  // next child on stack is not null
    next_position_ = child;
  }
  base_type::position(next_position_);
  if (next_position_->second.links().size() > 1) {  // traverse right branch
    auto link = *(++next_position_->second.links().begin());
    child = vertices().find(link);
    e = edge<Container>(next_position_->first, link);
    if (child != vertices().end() &&
        is_traversable(e)) {  // don't move to right child if null
      to_visit_.push(e);
      next_position_ = child;
    }
  }
  return true;
}

}  // namespace vertex
