#pragma once

#include <vertex/traversal.h>
#include <memory>
#include <queue>

namespace vertex {
/**@TODO Allow construction AT a given parent/child. Store child link pointer.
 * Ensure Iterator can be used to construct container of values */

/** Bredth first tree traversal */
template <typename Container>
class breadth_first_traversal
    : public traversal<Container, breadth_first_traversal<Container>> {
 public:
  using self_type = breadth_first_traversal<Container>;
  using base_type = traversal<Container, self_type>;
  using base_type::base_type;
  using base_type::is_traversable;
  using base_type::position;
  using base_type::vertices;
  using typename base_type::predicate_type;

  bool next();

 private:
  std::queue<typename base_type::edge_type> to_visit_;
};

template <typename Container>
bool breadth_first_traversal<Container>::next() {
  auto result = false;
  if (position() != vertices().end()) {
    for (const auto& child : position()->second) {
      auto e = edge<Container>(position()->first, child);
      if (is_traversable(e)) {
        to_visit_.push(e);
      }
    }
  } else {
    result = false;
  }
  while (!to_visit_.empty()) {
    auto edge = to_visit_.front();
    to_visit_.pop();
    base_type::position(vertices().find(edge.target()));
    if (position() != vertices().end()) {
      result = true;
      break;
    }
  }
  return result;
}

}  // namespace vertex
