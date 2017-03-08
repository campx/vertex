#pragma once
#include <algorithm>
#include <cassert>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <type_traits>

namespace merkle
{

/** NodeStore is an AssociativeContainer whose keys are hashes and whose
 * values are hash,node pairs
 * LinkStore is an AssociativeContainer whose keys are hashes and whose values
 * are hash,Containers-of-hashes pairs
 * NodeStore must not invalidate iterators on insertion or deletion */
template <typename NodeStore, typename LinkStore>
class Forest
{
public:
    /** Node stored in the tree */
    using link_container = LinkStore;
    using node_container = NodeStore;
    using value_type = typename NodeStore::value_type;
    /** Hash which identifies nodes */
    using key_type = typename NodeStore::key_type;
    using node_iterator = typename NodeStore::const_iterator;
    using link_iterator = typename LinkStore::const_iterator;
    using node_type = typename NodeStore::value_type::second_type;
    using link_type = typename LinkStore::value_type;

    static_assert(std::is_same<typename NodeStore::key_type,
                               typename LinkStore::key_type>::value,
                  "NodeStore::key_type != LinkStore::key_type");
    static_assert(
        std::is_same<typename LinkStore::key_type,
                     typename LinkStore::value_type::second_type>::value,
        "LinkStore::key_type != LinkStore::value_type::second_type");

    explicit Forest(NodeStore nodes = NodeStore(),
                    LinkStore links = LinkStore());

    /** Get a read-only handle to the data structure containing all nodes
     * for this graph */
    const NodeStore& nodes() const
    {
        return nodes_;
    }

    /** Get a read-only handle to the data structure containing all links
     * for this graph */
    const LinkStore& links() const
    {
        return links_;
    }

    /** Insert a node into the graph, creating links from all its children
     * Every child must exist */
    std::pair<node_iterator, bool> insert(const node_type& node);

    /** Remove the node at the specified position
     * The position must be valid and the node must be an orphan **/
    node_iterator erase(node_iterator pos);

    /** Remove the given link */
    link_iterator erase(const link_type& link);

    /** Insert a link into the graph
     * Parent and child nodes must exist */
    std::pair<link_iterator, bool> insert(const link_type& link);

private:
    /** Get an iterator to the link matching the argument */
    link_iterator find(const link_type& link);

    /** Remove the node link at the given position.
     * The given position must be valid. */
    link_iterator erase(link_iterator pos);

    NodeStore nodes_;
    LinkStore links_;
};

template <typename N, typename L>
Forest<N, L>::Forest(N nodes, L links)
    : nodes_(std::move(nodes)), links_(std::move(links))
{
}

template <typename N, typename L>
std::pair<typename Forest<N, L>::node_iterator, bool>
Forest<N, L>::insert(const typename N::value_type::second_type& node)
{
    auto hash_node = std::make_pair(node.hash(), node);
    auto result = nodes_.insert(hash_node); /** store the node */
    for (const auto& link : node.links())
    { // add a link from node to each of its children
        insert(link_type(link, node.hash()));
    }
    return result;
}

template <typename N, typename L>
typename Forest<N, L>::link_iterator
Forest<N, L>::erase(typename Forest<N, L>::link_iterator pos)
{
    auto result = pos;
    std::queue<link_iterator> to_visit;
    to_visit.push(pos);
    while (!to_visit.empty())
    {
        pos = to_visit.front();
        to_visit.pop();
        assert(pos != links().end());
        auto child = pos->first;
        result = links_.erase(pos);
        if (0 == links().count(child))
        {
            auto node = nodes().find(child);
            if (nodes().end() != node)
            {
                for (const auto& grandchild : node->second.links())
                {
                    auto it = find(link_type(grandchild, node->first));
                    to_visit.push(it);
                }
                nodes_.erase(node);
            }
        }
    }
    return result;
}

template <typename N, typename L>
typename Forest<N, L>::node_iterator
Forest<N, L>::erase(typename Forest<N, L>::node_iterator pos)
{
    auto result = pos;
    const auto& hash = pos->first;
    const auto& node = pos->second;
    assert(node.hash() == hash);

    if (links().count(hash) == 0)
    { // Erase node iff no references to it
        for (const auto& child : node.links())
        { // remove links from node to its children
            auto child_link = link_type(child, hash);
            erase(child_link);
        }
        result = nodes_.erase(pos);
    }
    return result;
}

template <typename N, typename L>
typename Forest<N, L>::link_iterator
Forest<N, L>::find(const typename Forest<N, L>::link_type& link)
{
    auto result = links().cend();
    auto range = links().equal_range(link.first);
    auto it = std::find(range.first, range.second, link);
    if (it != range.second)
    {
        result = it;
    }
    return result;
}

template <typename N, typename L>
std::pair<typename Forest<N, L>::link_iterator, bool>
Forest<N, L>::insert(const typename Forest<N, L>::link_type& link)
{
    assert(nodes().find(link.first) != nodes().end());  // Parent must exist
    assert(nodes().find(link.second) != nodes().end()); // Child too
    auto it = find(link);
    auto inserted = false;
    if (it == links().end())
    {
        it = links_.insert(link);
        inserted = true;
    }
    return std::make_pair(it, inserted);
}

template <typename N, typename L>
typename Forest<N, L>::link_iterator
Forest<N, L>::erase(const typename Forest<N, L>::link_type& link)
{
    auto result = find(link);
    if (result != links().end())
    {
        result = erase(result);
    }
    return result;
}

} // namespace merkle
