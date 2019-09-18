#include <vertex/LinkIterator.h>
#include <cassert>
#include <memory>

namespace vertex {
/******************
 * A_________     *
 *  \  \  \  \    *
 *   \  \  \  \   *
 *    \  \  \  \  *
 *     B  C  D  E *
 *****************/
/** Array provides a std::vector like interface atop a tree of depth 1 */
template <typename Container>
class Array {
 public:
  // typedefs
  using container_type = Container;
  using value_type = std::pair<typename container_type::key_type,
                               typename container_type::mapped_type>;
  using node_type = typename value_type::second_type;
  using link_type = typename node_type::link_type;
  using size_type = typename node_type::container_type::size_type;
  using allocator_type = typename node_type::container_type::allocator_type;
  using key_type = typename node_type::link_type;
  using difference_type = typename node_type::container_type::difference_type;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;

  using iterator = vertex::LinkIterator<Container>;

  // methods
  Array();
  explicit Array(Container& container, value_type root = value_type());

  iterator begin() const;
  iterator end() const;

  bool empty() const;
  size_type length() const;
  void clear();
  void root(const value_type& value);
  iterator insert(iterator pos, const value_type& value);
  void push_back(const value_type& value);

  const value_type& root() const;

 private:  // data
  Container* container_;
  value_type root_;
};

template <typename Container>
Array<Container>::Array() {}

template <typename Container>
Array<Container>::Array(Container& container, value_type root)
    : container_(&container), root_(std::move(root)) {}

template <typename Container>
typename Array<Container>::iterator Array<Container>::begin() const {
  return iterator(*container_, &root_.second, root_.second.links().begin());
}

template <typename Container>
typename Array<Container>::iterator Array<Container>::end() const {
  return std::end(begin());
}

template <typename Container>
bool Array<Container>::empty() const {
  return root_.second.links().empty();
}

template <typename Container>
typename Array<Container>::size_type Array<Container>::length() const {
  return root_.second.links().size();
}

template <typename Container>
void Array<Container>::clear() {
  root_.second.links().clear();
}

template <typename Container>
const typename Array<Container>::value_type& Array<Container>::root() const {
  return root_;
}

template <typename Container>
void Array<Container>::push_back(
    const typename Array<Container>::value_type& value) {
  insert(end(), value);
}

template <typename Container>
typename Array<Container>::iterator Array<Container>::insert(
    typename Array<Container>::iterator pos,
    const typename Array<Container>::value_type& value) {
  (*container_)[value.first] = value.second;
  auto result = root_.second.links().insert(pos.position(), value.first);
  return iterator(*container_, &root_.second, result);
}

template <typename Container>
void Array<Container>::root(const value_type& value) {
  assert(container_);
  root_ = value;
}

}  // namespace vertex
