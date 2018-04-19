#pragma once
#include <functional>
#include <utility>

namespace vertex
{

/** Applies a predicate to pairs of values to those within a given sequence
 *
 * Whenever a match occurs, position markers within the sequence advance
 *
 * Example:
 * sequence = "/", "home", "/", "user"
 * input = ("home", "/")
 * result = true
 * */
template <typename Iterator,
          typename Predicate = std::equal_to<typename Iterator::value_type>>
class PathPredicate
{
public: /** Type definitions */
    using value_type = typename std::iterator_traits<Iterator>::value_type;
    using argument_type = std::pair<value_type, value_type>;
    using result_type = bool;

public: /** Constructors */
    explicit PathPredicate(Iterator begin = Iterator(),
                           Iterator end = Iterator(),
                           Predicate predicate = Predicate());

public: /** Operators */
    result_type operator()(const argument_type& input);

private:                  /** Data */
    Iterator begin_;      /**< Beginning of sequence */
    Iterator end_;        /**< End of sequence */
    Predicate predicate_; /**< Predicate to match values */
    Iterator current_it_; /**< Current value in sequence */
    Iterator next_it_;    /**< Next value in sequence */
};

template <typename Iterator, typename Predicate>
PathPredicate<Iterator, Predicate>
makePathPredicate(Iterator begin, Iterator end, Predicate predicate);

template <typename Iterator, typename Predicate>
PathPredicate<Iterator, Predicate>::PathPredicate(Iterator begin,
                                                  Iterator end,
                                                  Predicate predicate)
    : begin_(std::move(begin)), end_(std::move(end)),
      predicate_(std::move(predicate)), current_it_(begin_), next_it_(begin_)
{
    if (begin_ != end_)
    {
        ++next_it_;
    }
}

template <typename Iterator, typename Predicate>
PathPredicate<Iterator, Predicate>
makePathPredicate(Iterator begin, Iterator end, Predicate predicate)
{
    return PathPredicate<decltype(begin), decltype(predicate)>(
        std::forward<Iterator>(begin), std::forward<Iterator>(end),
        std::forward<Predicate>(predicate));
}

template <typename Iterator, typename Predicate>
typename PathPredicate<Iterator, Predicate>::result_type
PathPredicate<Iterator, Predicate>::operator()(
    const typename PathPredicate<Iterator, Predicate>::argument_type& input)
{
    auto result = next_it_ != end_ && predicate_(input.first, *current_it_) &&
                  predicate_(input.second, *next_it_);
    if (result)
    {
        current_it_++;
        next_it_++;
    }
    return result;
}

} // namespace vertex
