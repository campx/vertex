#pragma once
#include <vertex/PreOrderTraversal.h>

namespace vertex {

/** LinkedList is a singly-linked list.
 * It provides fast insertion for append-only applications such as
 * block chains.
 *
 * Each element has a back pointer to the previous element.
 * Example use case: time series
 *  O<--O<--O<--O<--O
 *  1   2   3   4   5
 */
template <typename Container>
class LinkedList {
 public:
  using value_type = typename Container::value_type;
  using size_type = std::size_t;
  using difference_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using iterator = PreOrderTraversal<Container>;
};

}  // namespace vertex
