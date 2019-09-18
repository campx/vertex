#pragma once
#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

namespace toolbox {
/** Fills a std::vector<Iterator>, which can then be
're-wound' with operator-- even in forward-only iterators like graph search */
template <typename Iterator>
class IteratorRecorder : public std::iterator<std::input_iterator_tag,
                                              typename Iterator::value_type> {
 public: /** Type Definitions */
  using reference = typename Iterator::reference;
  using value_type = typename Iterator::value_type;

 private:                                           /** Data */
  std::shared_ptr<std::vector<value_type>> values_; /** Cache */
  Iterator it_;
  typename std::vector<Iterator>::size_type index_; /** Current position */

 private:
  void evaluate();
  value_type& get();

 public: /** Constructors */
  IteratorRecorder();
  explicit IteratorRecorder(Iterator it);

 public: /** Operators */
  IteratorRecorder& operator++();

  const IteratorRecorder operator++(int dummy);
  IteratorRecorder& operator--();

  const IteratorRecorder operator--(int dummy);
  value_type& operator*();
  value_type* operator->();
  bool operator==(const IteratorRecorder& rhs) const;
  bool operator!=(const IteratorRecorder& rhs) const;
  bool operator==(const Iterator& rhs) const;
  bool operator!=(const Iterator& rhs) const;
};

template <typename Iterator>
bool operator==(const Iterator& lhs, const IteratorRecorder<Iterator>& rhs);

template <typename Iterator>
bool operator!=(const Iterator& lhs, const IteratorRecorder<Iterator>& rhs);

/********************************IMPLEMENTATION********************************/

template <typename Iterator>
IteratorRecorder<Iterator>::IteratorRecorder() {}

template <typename Iterator>
IteratorRecorder<Iterator>::IteratorRecorder(Iterator it)
    : values_(std::make_shared<std::vector<value_type>>()),
      it_(std::move(it)),
      index_(0) {}

template <typename Iterator>
void IteratorRecorder<Iterator>::evaluate() {
  if (index_ >= values_->size()) {
    values_->emplace_back(*it_);
  }
}

template <typename Iterator>
typename IteratorRecorder<Iterator>::value_type&
IteratorRecorder<Iterator>::get() {
  evaluate();
  auto& result((*values_)[index_]);
  return result;
}

template <typename Iterator>
IteratorRecorder<Iterator>& IteratorRecorder<Iterator>::operator++() {
  evaluate();
  ++index_;
  ++it_;
  return *this;
}

template <typename Iterator>
const IteratorRecorder<Iterator> IteratorRecorder<Iterator>::operator++(
    int dummy) {
  (void)dummy;
  auto result = *this;
  ++*this;
  return result;
}

template <typename Iterator>
IteratorRecorder<Iterator>& IteratorRecorder<Iterator>::operator--() {
  --index_;
  return *this;
}

template <typename Iterator>
const IteratorRecorder<Iterator> IteratorRecorder<Iterator>::operator--(
    int dummy) {
  (void)dummy;
  auto result = *this;
  --*this;
  return result;
}

template <typename Iterator>
typename IteratorRecorder<Iterator>::value_type&
    IteratorRecorder<Iterator>::operator*() {
  return get();
}

template <typename Iterator>
typename IteratorRecorder<Iterator>::value_type*
    IteratorRecorder<Iterator>::operator->() {
  return &get();
}

template <typename Iterator>
bool IteratorRecorder<Iterator>::operator==(
    const IteratorRecorder<Iterator>& rhs) const {
  /** Scenarios:
   * Iterators equal but different indexes => false
   * Iterators different but same index and values => true
   **/
  auto result = false;

  if (!values_ && !rhs.values_) {
    result = true;
  } else if ((values_ && !rhs.values_) || (rhs.values_ && !values_)) {
    result = false;
  } else if (values_->empty() ||
             rhs.values_->empty()) {  // compare when initialised with iterator
    result = it_ == rhs.it_;
  } else if (values_ == rhs.values_) {
    result = index_ == rhs.index_;
  } else if (index_ == rhs.index_) {
    auto begin = values_->begin();
    auto end = std::next(begin, index_);
    result = std::equal(begin, end, rhs.values_->begin());
  }
  return result;
}

template <typename Iterator>
bool IteratorRecorder<Iterator>::operator!=(
    const IteratorRecorder<Iterator>& rhs) const {
  return !(*this == rhs);
}

template <typename Iterator>
bool IteratorRecorder<Iterator>::operator==(const Iterator& rhs) const {
  return it_ == rhs;
}

template <typename Iterator>
bool IteratorRecorder<Iterator>::operator!=(const Iterator& rhs) const {
  return !(*this == rhs);
}

template <typename Iterator>
bool operator==(const Iterator& lhs, const IteratorRecorder<Iterator>& rhs) {
  return rhs == lhs;
}

template <typename Iterator>
bool operator!=(const Iterator& lhs, const IteratorRecorder<Iterator>& rhs) {
  return rhs != lhs;
}
}  // namespace toolbox
