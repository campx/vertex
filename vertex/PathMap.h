#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
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

        explicit Decoder(key_type path);

        value_type operator()(const typename Container::value_type& value);

    private:
        key_type path_;
        key_type prev_links_;
        key_type links_;
    };

    using Predicate =
    toolbox::SequencePredicate<typename key_type::const_iterator, Compare>;
    using Traversal = BreadthFirstTraversal<Container, Predicate>;
    using Transformer = toolbox::IteratorTransformer<Decoder, Traversal>;
    using iterator = toolbox::IteratorRecorder<Transformer>;

    PathMap(const Container& nodes, typename Container::const_iterator root,
            Compare compare = Compare());

    const typename Container::const_iterator& root() const;

    iterator begin() const;

    iterator end() const;

    /** Search for a path with partial matching
     * @return Value containing all matched path segments */
    iterator search(const key_type& p) const;

    /** Find a full path
     * @return Value of match or end() if the case of an incomplete match */
    iterator find(const key_type& p) const;

    iterator erase(const iterator& it);

    std::pair<iterator, bool> insert(const value_type& value);

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
PathMap<Container, Compare>::Decoder::operator()(
    const typename Container::value_type& value)
{

    auto prev = std::find(prev_links_.begin(), prev_links_.end(), value.first);
    if (prev != prev_links_.end())
    {
        path_.erase(--path_.end());
    }
    path_.push_back(value.first);
    prev_links_ = links_;
    links_ = value.second.links();
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
PathMap<Container, Compare>::begin() const
{
    auto first = Traversal(*nodes_, root_);
    auto last = first.end();
    first = first == last ? first : ++first; // don't include root in results
    auto decoder = Decoder(key_type{root()->first});
    auto transformer = Transformer(first, decoder);
    return iterator(transformer);
}

template <typename Container, typename Compare>
typename PathMap<Container, Compare>::iterator
PathMap<Container, Compare>::end() const
{
    auto traversal = Traversal(*nodes_, root_).end();
    auto decoder = Decoder(key_type{root()->first});
    auto transformer = Transformer(traversal, decoder);
    return iterator(transformer);
}

template <typename Container, typename Compare>
typename PathMap<Container, Compare>::iterator
PathMap<Container, Compare>::search(const key_type& p) const
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

template <typename Container, typename Compare>
typename PathMap<Container, Compare>::iterator
PathMap<Container, Compare>::find(const key_type& p) const
{
    auto match = search(p);
    if (match->first.size() != p.size())
    {
        match = end();
    }
    return match;
}

template <typename Container, typename Compare>
std::pair<typename PathMap<Container, Compare>::iterator, bool>
PathMap<Container, Compare>::insert(const value_type& value)
{
    auto result = std::make_pair(search(value.first), false);
    if (result.first != end() || value.first.empty())
    {
        return result;
    }
    /*
    result.second = true;
    // Insert child node
    auto link = *(value.first.rbegin());
    auto chit = nodes_->find(link);
    if (chit == nodes_->end())
    {
        auto child = std::make_pair(link, value.second);
        nodes_->insert(child);
    }
    else
    {
        *chit = value.second;
    }
    // Walk back through tree to root
    auto begin = ++(value.first.rbegin()); // OK - already checked path is non-empty
    auto end = value.first.rend();
    for (auto it = begin; it != end; ++it)
    {
        if (result.first != begin() && result.first->first == *it)
        {
            auto& parent = result.first->second;
            if (parent.links().end() ==
                std::find(parent.links().begin(), parent.links().end()))
            {
                parent.links().push_back(chit->first);
            }
        }
        std::cout << *it << std::endl;
    }
     */
    return result;
}

} // namespace vertex
