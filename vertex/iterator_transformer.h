#pragma once

#include <memory>

namespace toolbox {

template <typename Transform, typename Iterator>
class iterator_transformer
    : public std::iterator<std::input_iterator_tag,
                           decltype(
                               Transform()(typename Iterator::value_type()))> {
 public:
  using self_type = iterator_transformer;

  explicit iterator_transformer(Iterator it = Iterator{},
                                Transform transform = Transform{});

  iterator_transformer(const iterator_transformer& value) = default;

  ~iterator_transformer() = default;

  iterator_transformer(iterator_transformer&&) = default;

  iterator_transformer& operator=(const iterator_transformer&) = default;

  iterator_transformer& operator=(iterator_transformer&&) = default;

  Iterator get() const;

  using value_type = decltype(Transform()(typename Iterator::value_type()));
  using reference = value_type&;
  using const_reference = typename std::add_const<reference>::type;
  using pointer = value_type*;
  using const_pointer = value_type const*;

  self_type operator++();

  const self_type operator++(int dummy);

  reference operator*();

  const_reference operator*() const;

  pointer operator->();

  const_pointer operator->() const;

  bool operator==(const iterator_transformer& rhs) const;

  bool operator!=(const iterator_transformer& rhs) const;

 private:
  Iterator it_;
  Transform transform_;
  mutable value_type value_;
  mutable bool dirty_flag_;

  void evaluate();

  void increment();
};

template <typename Transform, typename Iterator>
iterator_transformer<Transform, Iterator>::iterator_transformer(
    Iterator it, Transform transform)
    : it_(std::move(it)), transform_(transform), dirty_flag_(true) {}

template <typename Transform, typename Iterator>
Iterator iterator_transformer<Transform, Iterator>::get() const {
  return it_;
}

template <typename Transform, typename Iterator>
void iterator_transformer<Transform, Iterator>::increment() {
  ++it_;
  dirty_flag_ = true;
}

template <typename Transform, typename Iterator>
typename iterator_transformer<Transform, Iterator>::self_type
iterator_transformer<Transform, Iterator>::operator++() {
  increment();
  return *this;
}

template <typename Transform, typename Iterator>
const typename iterator_transformer<Transform, Iterator>::self_type
iterator_transformer<Transform, Iterator>::operator++(int dummy) {
  (void)dummy;
  auto tmp = *this;
  increment();
  return tmp;
}

template <typename Transform, typename Iterator>
void iterator_transformer<Transform, Iterator>::evaluate() {
  if (dirty_flag_) {
    value_ = transform_(*it_);
    dirty_flag_ = false;
  }
}

template <typename Transform, typename Iterator>
typename iterator_transformer<Transform, Iterator>::reference
    iterator_transformer<Transform, Iterator>::operator*() {
  evaluate();
  return value_;
}

template <typename Transform, typename Iterator>
typename iterator_transformer<Transform, Iterator>::const_reference
    iterator_transformer<Transform, Iterator>::operator*() const {
  if (dirty_flag_) {
    value_ = transform_(*it_);
    dirty_flag_ = false;
  }
  return value_;
}

template <typename Transform, typename Iterator>
typename iterator_transformer<Transform, Iterator>::pointer
    iterator_transformer<Transform, Iterator>::operator->() {
  evaluate();
  return &value_;
}

template <typename Transform, typename Iterator>
typename iterator_transformer<Transform, Iterator>::const_pointer
    iterator_transformer<Transform, Iterator>::operator->() const {
  if (dirty_flag_) {
    value_ = transform_(*it_);
    dirty_flag_ = false;
  }
  return &value_;
}

template <typename Transform, typename Iterator>
bool iterator_transformer<Transform, Iterator>::operator==(
    const iterator_transformer<Transform, Iterator>& rhs) const {
  return it_ == rhs.it_;
}

template <typename Transform, typename Iterator>
bool iterator_transformer<Transform, Iterator>::operator!=(
    const iterator_transformer<Transform, Iterator>& rhs) const {
  return it_ != rhs.it_;
}

}  // namespace toolbox
