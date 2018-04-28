#pragma once
#include <algorithm>
#include <functional>
#include <toolbox/Iterator.h>
#include <toolbox/IteratorRecorder.h>
#include <toolbox/IteratorTransformer.h>
#include <toolbox/SequencePredicate.h>
#include <vector>
#include <vertex/BreadthFirstTraversal.h>

namespace vertex
{

/** PathMap traverses a graph, matching a Link sequence using a given
 * Compare function, returning an array of matching Link,Node pairs */
template <typename Container,
          typename Compare = std::equal_to<typename Container::key_type>>
class PathMap
{
public:
    using key_type = std::vector<typename Container::key_type>;
    using mapped_type = typename Container::mapped_type;
    using value_type = std::pair<key_type, mapped_type>;

    class Decoder
    {
    public:
        Decoder() = default;
        Decoder(key_type path);
        value_type operator()(const typename Container::value_type& value);

    private:
        key_type path_;
    };

    using Predicate =
        toolbox::SequencePredicate<typename key_type::const_iterator, Compare>;
    using Traversal = BreadthFirstTraversal<Container, Predicate>;
    using Transformer = toolbox::IteratorTransformer<Decoder, Traversal>;
    using iterator = toolbox::IteratorRecorder<Transformer>;

    PathMap(const Container& nodes,
            typename Container::const_iterator root,
            Compare compare = Compare());
    const typename Container::const_iterator& root() const;

    iterator find(const key_type& p) const;
    iterator erase(const iterator& it);
    iterator insert(iterator hint, const value_type& value);

private:
    const Container* nodes_;
    typename Container::const_iterator root_;
    Compare compare_;
};

template <typename Container, typename Compare>
PathMap<Container, Compare>::PathMap(const Container& nodes,
                                     typename Container::const_iterator root,
                                     Compare compare)
    : nodes_(&nodes), root_(std::move(root)), compare_(std::move(compare))
{
}

template <typename Container, typename Compare>
PathMap<Container, Compare>::Decoder::Decoder(key_type path)
    : path_(std::move(path))
{
}

template <typename Container, typename Compare>
typename PathMap<Container, Compare>::value_type
PathMap<Container, Compare>::Decoder::
operator()(const typename Container::value_type& value)
{
    path_.push_back(value.first);
    return std::make_pair(path_, value.second);
}

template <typename Container, typename Compare>
const typename Container::const_iterator&
PathMap<Container, Compare>::root() const
{
    return root_;
}

template <typename Container, typename Compare>
typename PathMap<Container, Compare>::iterator
PathMap<Container, Compare>::find(const key_type& p) const
{
    auto predicate = Predicate(p.begin(), p.end(), compare_);
    auto first = Traversal(*nodes_, root_, predicate);
    auto last = first.end();
    first = first == last ? first : ++first; // don't include root in results
    auto decoder = Decoder(key_type{root()->first});
    auto first_transformer = Transformer(first, decoder);
    auto last_transformer = Transformer(last, decoder);
    auto begin = iterator(first_transformer);
    auto end = iterator(last_transformer);
    auto result = toolbox::back(begin, end);
    return result;
}

} // namespace vertex
