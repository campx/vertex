#pragma once
#include <algorithm>
#include <functional>
#include <vector>
#include <vertex/BreadthFirstTraversal.h>
#include <vertex/PathPredicate.h>

namespace vertex
{

/** PathMap traverses a graph, matching a Link sequence using a given
 * Compare function, returning an array of matching Link,Node pairs */
template <typename Container,
          typename Iterator,
          typename Compare = std::equal_to<typename Container::key_type>>
class PathMap
{
public:
    using const_iterator = typename Container::const_iterator;
    using value_type = typename Container::value_type;
    using result_type = std::vector<value_type>;
    PathMap(const Container& nodes, const_iterator root);

    result_type operator()(const Iterator& begin, const Iterator& end) const;

private:
    const Container* nodes_;
    const_iterator root_;
};

template <typename Container, typename Iterator, typename Compare>
PathMap<Container, Iterator, Compare>::PathMap(
    const Container& nodes, typename Container::const_iterator root)
    : nodes_(&nodes), root_(std::move(root))
{
}

template <typename Container, typename Iterator, typename Compare>
typename PathMap<Container, Iterator, Compare>::result_type
PathMap<Container, Iterator, Compare>::
operator()(const Iterator& begin, const Iterator& end) const
{
    using Predicate = PathPredicate<Iterator, Compare>;
    using Traversal = BreadthFirstTraversal<Container, Predicate>;
    auto predicate = Predicate(begin, end);
    auto first = Traversal(*nodes_, root_, predicate);
    auto last = first.end();
    first = first == last ? first : ++first; // don't include root in results
    auto result = result_type{};
    std::copy(first, last, std::back_inserter<result_type>(result));
    return result;
}

} // namespace vertex
