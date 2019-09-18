#pragma once
#include <functional>
#include <utility>

namespace toolbox {

/** Applies a comparison to pairs of values to those within a given sequence
 *
 * Whenever a match occurs, position markers within the sequence advance
 * If the sequence is empty, return true for every call
 *
 * Example:
 * sequence = "/", "home", "/", "user"
 * input = ("home", "/")
 * result = true
 * */
template <typename Iterator,
          typename Compare = std::less<decltype(*Iterator())>>
class sequence_predicate {
 public:
  /** Type definitions */
  using value_type = decltype(*Iterator());
  using argument_type = std::pair<value_type, value_type>;
  using result_type = bool;

  /** Constructors */
  explicit sequence_predicate(Iterator begin = Iterator(),
                              Iterator end = Iterator(),
                             Compare compare = Compare());

  /** Operators */
  result_type operator()(const argument_type& input);

 private:
  /** Methods */
  [[nodiscard]] bool empty() const;

  /** Data */
  Iterator begin_;   /**< Beginning of sequence */
  Iterator end_;     /**< End of sequence */
  Compare compare_;  /**< Compare to match values */
  Iterator current_; /**< Current value in sequence */
  Iterator next_;    /**< Next value in sequence */
};

template <typename Iterator, typename Compare>
sequence_predicate<Iterator, Compare> makeSequencePredicate(Iterator begin,
                                                            Iterator end,
                                                           Compare compare);

template <typename Iterator, typename Compare>
sequence_predicate<Iterator, Compare>::sequence_predicate(Iterator begin,
                                                          Iterator end,
                                                        Compare compare)
    : begin_(std::move(begin)),
      end_(std::move(end)),
      compare_(std::move(compare)),
      current_(begin_),
      next_(current_) {
  if (!empty()) {
    ++next_;
  }
}

template <typename Iterator, typename Compare>
sequence_predicate<Iterator, Compare> makeSequencePredicate(Iterator begin,
                                                            Iterator end,
                                                           Compare compare) {
  return sequence_predicate<decltype(begin), decltype(compare)>(
      std::forward<Iterator>(begin), std::forward<Iterator>(end),
      std::forward<Compare>(compare));
}

template <typename Iterator, typename Compare>
bool sequence_predicate<Iterator, Compare>::empty() const {
  return begin_ == end_;
}

template <typename Iterator, typename Compare>
typename sequence_predicate<Iterator, Compare>::result_type
sequence_predicate<Iterator, Compare>::operator()(
    const typename sequence_predicate<Iterator, Compare>::argument_type&
        input) {
  auto result = empty();
  if (!result && next_ != end_) {
    auto a = compare_(input.first, *current_);
    auto b = compare_(*current_, input.first);
    auto c = compare_(input.second, *next_);
    auto d = compare_(*next_, input.second);
    result = !a && !b && !c && !d;
  }
  if (result) {
    current_ = current_ == end_ ? current_ : ++current_;
    next_ = next_ == end_ ? next_ : ++next_;
  }
  return result;
}

}  // namespace toolbox
