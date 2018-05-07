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

    PathMap(Container& nodes, typename Container::const_iterator root,
            Compare compare = Compare());

    const typename Container::const_iterator& root() const;

    Container& nodes() const;

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

    std::pair<typename Container::iterator, bool>
    insert_or_assign(const typename Container::key_type& key,
                     typename Container::mapped_type& value);

    Container* nodes_;
    typename Container::const_iterator root_;
    Compare compare_;
};

template <typename Container, typename Compare>
PathMap<Container, Compare>::PathMap(Container& nodes,
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
Container& PathMap<Container, Compare>::nodes() const
{
    return *nodes_;
};

template <typename Container, typename Compare>
typename PathMap<Container, Compare>::iterator
PathMap<Container, Compare>::begin() const
{
    auto first = Traversal(nodes(), root_);
    auto last = first.end();
    first = first == last ? first : ++first; // don't include root in results
    auto transformer = Transformer(first);
    return iterator(transformer);
}

template <typename Container, typename Compare>
typename PathMap<Container, Compare>::iterator
PathMap<Container, Compare>::end() const
{
    auto traversal = Traversal(nodes(), root_).end();
    auto transformer = Transformer(traversal);
    return iterator(transformer);
}

template <typename Container, typename Compare>
typename PathMap<Container, Compare>::iterator
PathMap<Container, Compare>::search(const key_type& p) const
{
    key_type path = key_type{root_->first}; // pre-pend root to search path
    path.insert(path.end(), p.begin(), p.end());
    auto predicate = Predicate(path.begin(), path.end(), compare_);
    auto first = Traversal(nodes(), root_, predicate);
    auto last = first.end();
    first = first == last ? first : ++first; // don't include root in results
    auto first_transformer = Transformer(first);
    auto last_transformer = Transformer(last);
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
    if (match != end() && match->first.size() != p.size())
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
    if (result.first != end() &&
        result.first->first.size() == value.first.size())
    { // already exists
        return result;
    }
    result.second = true;
    auto path_it = value.first.rbegin();
    auto child_it = nodes().end();
    auto node = value.second;
    for (auto rend = value.first.rend(); path_it != rend; ++path_it)
    {
        if (result.first != end())
        {
            if (compare_(result.first->first.back(), *path_it))
            { // Copy existing Node for this Link
                node = result.first->second;
                --result.first;
            }
        }
        else
        {
            node = mapped_type(); // create empty node
            if (child_it != nodes().end())
            { // add link from parent to child
                node.links().push_back(child_it->first);
            }
        }
        std::tie(child_it, result.second) = insert_or_assign(*path_it, node);
    }
    node = root_->second;
    if (node.links().end() ==
        std::find(node.links().begin(), node.links().end(), child_it->first))
    {
        node.links().push_back(child_it->first);
    }
    std::tie(root_, result.second) = insert_or_assign(child_it->first, node);
    result.first = search(value.first);
    return result;
}

template <typename Container, typename Compare>
std::pair<typename Container::iterator, bool>
PathMap<Container, Compare>::insert_or_assign(
    const typename Container::key_type& key,
    typename Container::mapped_type& value)
{
    auto result = std::make_pair(nodes().find(key), false);
    if (result.first == nodes().end())
    {
        result = nodes().emplace(std::make_pair(key, value));
    }
    else
    {
        result.first->second = value;
    }
    return result;
};

} // namespace vertex
