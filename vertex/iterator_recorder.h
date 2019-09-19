#pragma once
#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

namespace toolbox {
/** Fills a std::vector<Iterator>, which can then be
're-wound' with operator-- even in forward-only iterators like graph search */
template <typename Iterator>
class iterator_recorder {
 public: /** Type Definitions */
  using iterator_category = std::input_iterator_tag;
  using value_type = typename Iterator::value_type;
  using difference_type = std::ptrdiff_t;
  using pointer = typename Iterator::pointer;
  using reference = typename Iterator::reference;

 private:                                           /** Data */
  std::shared_ptr<std::vector<value_type>> values_; /** Cache */
  Iterator it_;
  typename std::vector<Iterator>::size_type index_; /** Current position */

 private:
  void evaluate();
  value_type& get();

 public: /** Constructors */
  iterator_recorder();
  explicit iterator_recorder(Iterator it);

 public: /** Operators */
  iterator_recorder& operator++();

  const iterator_recorder operator++(int dummy);
  iterator_recorder& operator--();

  const iterator_recorder operator--(int dummy);
  value_type& operator*();
  value_type* operator->();
  bool operator==(const iterator_recorder& rhs) const;
  bool operator!=(const iterator_recorder& rhs) const;
  bool operator==(const Iterator& rhs) const;
  bool operator!=(const Iterator& rhs) const;
};

template <typename Iterator>
bool operator==(const Iterator& lhs, const iterator_recorder<Iterator>& rhs);

template <typename Iterator>
bool operator!=(const Iterator& lhs, const iterator_recorder<Iterator>& rhs);

/********************************IMPLEMENTATION********************************/

template <typename Iterator>
iterator_recorder<Iterator>::iterator_recorder() {}

template <typename Iterator>
iterator_recorder<Iterator>::iterator_recorder(Iterator it)
    : values_(std::make_shared<std::vector<value_type>>()),
      it_(std::move(it)),
      index_(0) {}

template <typename Iterator>
void iterator_recorder<Iterator>::evaluate() {
  if (index_ >= values_->size()) {
    values_->emplace_back(*it_);
  }
}

template <typename Iterator>
typename iterator_recorder<Iterator>::value_type&
iterator_recorder<Iterator>::get() {
  evaluate();
  auto& result((*values_)[index_]);
  return result;
}

template <typename Iterator>
iterator_recorder<Iterator>& iterator_recorder<Iterator>::operator++() {
  evaluate();
  ++index_;
  ++it_;
  return *this;
}

template <typename Iterator>
const iterator_recorder<Iterator> iterator_recorder<Iterator>::operator++(
    int dummy) {
  (void)dummy;
  auto result = *this;
  ++*this;
  return result;
}

template <typename Iterator>
iterator_recorder<Iterator>& iterator_recorder<Iterator>::operator--() {
  --index_;
  return *this;
}

template <typename Iterator>
const iterator_recorder<Iterator> iterator_recorder<Iterator>::operator--(
    int dummy) {
  (void)dummy;
  auto result = *this;
  --*this;
  return result;
}

template <typename Iterator>
typename iterator_recorder<Iterator>::value_type& iterator_recorder<Iterator>::
operator*() {
  return get();
}

template <typename Iterator>
typename iterator_recorder<Iterator>::value_type* iterator_recorder<Iterator>::
operator->() {
  return &get();
}

template <typename Iterator>
bool iterator_recorder<Iterator>::operator==(
    const iterator_recorder<Iterator>& rhs) const {
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
bool iterator_recorder<Iterator>::operator!=(
    const iterator_recorder<Iterator>& rhs) const {
  return !(*this == rhs);
}

template <typename Iterator>
bool iterator_recorder<Iterator>::operator==(const Iterator& rhs) const {
  return it_ == rhs;
}

template <typename Iterator>
bool iterator_recorder<Iterator>::operator!=(const Iterator& rhs) const {
  return !(*this == rhs);
}

template <typename Iterator>
bool operator==(const Iterator& lhs, const iterator_recorder<Iterator>& rhs) {
  return rhs == lhs;
}

template <typename Iterator>
bool operator!=(const Iterator& lhs, const iterator_recorder<Iterator>& rhs) {
  return rhs != lhs;
}
}  // namespace toolbox
