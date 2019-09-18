#pragma once

#include <memory>

namespace toolbox {

template <typename Transform, typename Iterator>
class IteratorTransformer
    : public std::iterator<std::input_iterator_tag,
                           decltype(
                               Transform()(typename Iterator::value_type()))> {
 public:
  using self_type = IteratorTransformer;

  explicit IteratorTransformer(Iterator it = Iterator{},
                               Transform transform = Transform{});

  IteratorTransformer(const IteratorTransformer& value) = default;

  ~IteratorTransformer() = default;

  IteratorTransformer(IteratorTransformer&&) noexcept = default;

  IteratorTransformer& operator=(const IteratorTransformer&) = default;

  IteratorTransformer& operator=(IteratorTransformer&&) noexcept = default;

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

  bool operator==(const IteratorTransformer& rhs) const;

  bool operator!=(const IteratorTransformer& rhs) const;

 private:
  Iterator it_;
  Transform transform_;
  mutable value_type value_;
  mutable bool dirty_flag_;

  void evaluate();

  void increment();
};

template <typename Transform, typename Iterator>
IteratorTransformer<Transform, Iterator>::IteratorTransformer(
    Iterator it, Transform transform)
    : it_(std::move(it)), transform_(transform), dirty_flag_(true) {}

template <typename Transform, typename Iterator>
Iterator IteratorTransformer<Transform, Iterator>::get() const {
  return it_;
}

template <typename Transform, typename Iterator>
void IteratorTransformer<Transform, Iterator>::increment() {
  ++it_;
  dirty_flag_ = true;
}

template <typename Transform, typename Iterator>
typename IteratorTransformer<Transform, Iterator>::self_type
IteratorTransformer<Transform, Iterator>::operator++() {
  increment();
  return *this;
}

template <typename Transform, typename Iterator>
const typename IteratorTransformer<Transform, Iterator>::self_type
IteratorTransformer<Transform, Iterator>::operator++(int dummy) {
  (void)dummy;
  auto tmp = *this;
  increment();
  return tmp;
}

template <typename Transform, typename Iterator>
void IteratorTransformer<Transform, Iterator>::evaluate() {
  if (dirty_flag_) {
    value_ = transform_(*it_);
    dirty_flag_ = false;
  }
}

template <typename Transform, typename Iterator>
typename IteratorTransformer<Transform, Iterator>::reference
    IteratorTransformer<Transform, Iterator>::operator*() {
  evaluate();
  return value_;
}

template <typename Transform, typename Iterator>
typename IteratorTransformer<Transform, Iterator>::const_reference
    IteratorTransformer<Transform, Iterator>::operator*() const {
  if (dirty_flag_) {
    value_ = transform_(*it_);
    dirty_flag_ = false;
  }
  return value_;
}

template <typename Transform, typename Iterator>
typename IteratorTransformer<Transform, Iterator>::pointer
    IteratorTransformer<Transform, Iterator>::operator->() {
  evaluate();
  return &value_;
}

template <typename Transform, typename Iterator>
typename IteratorTransformer<Transform, Iterator>::const_pointer
    IteratorTransformer<Transform, Iterator>::operator->() const {
  if (dirty_flag_) {
    value_ = transform_(*it_);
    dirty_flag_ = false;
  }
  return &value_;
}

template <typename Transform, typename Iterator>
bool IteratorTransformer<Transform, Iterator>::operator==(
    const IteratorTransformer<Transform, Iterator>& rhs) const {
  return it_ == rhs.it_;
}

template <typename Transform, typename Iterator>
bool IteratorTransformer<Transform, Iterator>::operator!=(
    const IteratorTransformer<Transform, Iterator>& rhs) const {
  return it_ != rhs.it_;
}

}  // namespace toolbox
