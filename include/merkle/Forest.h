#pragma once
#include <algorithm>
#include <cassert>
#include <map>
#include <memory>
#include <queue>
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
    const NodeStore& nodes() const { return nodes_; }

    /** Get a read-only handle to the data structure containing all links
     * for this graph */
    const LinkStore& links() const { return links_; }

    /** Insert a node into the graph, creating links from all its children
     * Every child must exist */
    std::pair<node_iterator, bool> insert(const node_type& node);

    /** Insert a link from a child node to a parent node in a tree
     * The nodes referenced by the link and root must exist
     * @return Updated root node, link between child and parent and a boolean
     * to indicate whether an insertion occurred */
    std::tuple<node_iterator, link_iterator, bool>
    insert(node_iterator root, node_iterator parent, node_iterator child);

    /** Remove the node link at the given position.
     * The given position must be valid. */
    link_iterator erase(link_iterator pos);

    link_iterator erase(const link_type& link);

private:
    /** Insert a link into the graph
     * Parent and child nodes must exist */
    std::pair<link_iterator, bool> insert(const link_type& link);

    /** Remove the node at the specified position
     * The position must be valid **/
    node_iterator erase(node_iterator pos);

    NodeStore nodes_;
    LinkStore links_;
};

/** @TODO Pin class which inserts a self-edge on construction and erases on
 * destruction. Insert a new pin whenever child doesn't equal most recently
 * inserted pin */

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
    for (const auto& child : node)
    { // add a link from node to each of its children
        insert(link_type(child, node.hash()));
    }
    return result;
}

template <typename N, typename L>
typename std::tuple<typename Forest<N, L>::node_iterator,
                    typename Forest<N, L>::link_iterator, bool>
Forest<N, L>::insert(typename Forest<N, L>::node_iterator root,
                     typename Forest<N, L>::node_iterator parent,
                     typename Forest<N, L>::node_iterator child)
{ /* Follow links up the tree and speculatively generate new branches. When
    the path terminates, check if the endpoint is root, if not, unlink */
    using node_iterator = Forest<N, L>::node_iterator;
    std::vector<link_type> pins; // pins prevent deletion of subtree
    pins.push_back(link_type(child->first, child->first));

    // Map from node to updated version, with bool to indicate whether it's new
    auto compare = [](node_iterator a, node_iterator b) {
        return a->first < b->first;
    };
    std::map<node_iterator, node_iterator, decltype(compare)> updated(compare);

    // Queue of child-parent pairs to visit */
    std::queue<std::pair<node_iterator, node_iterator>> to_visit;
    to_visit.push(std::make_pair(child, parent));

    while (!to_visit.empty())
    {
        auto next_pair = to_visit.front();
        to_visit.pop();
        const auto source_child = next_pair.first;
        const auto source_parent = next_pair.second;
        auto target_parent = source_parent;
        auto target_child = source_child;
        {
            auto updated_parent = updated.find(source_parent);
            auto updated_child = updated.find(source_child);
            if (updated_child != updated.end())
            {
                target_child = updated_child->second;
                target_child = nodes().find(updated_child->second->first);
            }
            if (updated_parent != updated.end())
            {
                target_parent = updated_parent->second;
                target_parent = nodes().find(updated_parent->second->first);
            }
        }
        if (target_parent != nodes().end())
        {
            if (target_child->first != pins.back().first)
            {
                pins.push_back(
                    link_type(target_child->first, target_child->first));
                insert(pins.back());
            }
            auto node = target_parent->second;
            node.erase(source_child->first);
            node.insert(target_child->first);
            auto inserted_parent = insert(node);
            auto range = links_.equal_range(source_parent->second.hash());
            for (auto it = range.first; it != range.second; ++it)
            { // enqueue grandparents
                auto grandparent = nodes().find(it->second);
                assert(grandparent != nodes().end());
                to_visit.push(std::make_pair(source_parent, grandparent));
            }
            if (range.first == range.second)
            { // no parents, so this is a source node
                if (root == target_parent)
                { // update root
                    root = inserted_parent.first;
                    target_parent = root;
                }
                else if (inserted_parent.second)
                { // remove entire node if new
                    erase(inserted_parent.first);
                }
                else
                { // remove new link to node
                    auto link = link_type(target_child->first,
                                          inserted_parent.first->first);
                    erase(link);
                }
            }
            else
            {
                target_parent = inserted_parent.first;
            }
            auto existing_mapping = updated.find(source_parent);
            if (existing_mapping != updated.end())
            {
                if (existing_mapping->second != target_parent)
                { // remove existing mapping
                    updated.erase(existing_mapping);
                    existing_mapping = updated.end();
                }
            }
            if (existing_mapping == updated.end() &&
                source_parent != target_parent)
            {
                updated.insert(std::make_pair(source_parent, target_parent));
            }
        }
    }
    auto updated_parent = updated.find(parent);
    auto parent_was_updated = updated_parent != updated.end();
    if (parent_was_updated)
    {
        parent = updated_parent->second;
    }
    auto link = link_type(child->first, parent->first);
    auto range = links_.equal_range(child->first);
    auto child_link = std::find(range.first, range.second, link);
    assert(child_link != range.second); // link must exist
    for (auto link_pin : pins)
    {
        erase(link_pin);
    }
    return std::make_tuple(root, child_link, parent_was_updated);
}

template <typename N, typename L>
typename Forest<N, L>::link_iterator
Forest<N, L>::erase(typename Forest<N, L>::link_iterator pos)
{
    auto child = pos->first;
    auto result = links_.erase(pos);
    if (0 == links_.count(child))
    {
        auto node = nodes().find(child);
        if (nodes().end() != node)
        {
            erase(node);
        }
    }
    return result;
}

template <typename N, typename L>
std::pair<typename Forest<N, L>::link_iterator, bool>
Forest<N, L>::insert(const typename Forest<N, L>::link_type& link)
{
    assert(nodes().find(link.first) != nodes().end());  // Parent must exist
    assert(nodes().find(link.second) != nodes().end()); // Child too
    auto count = links_.count(link.first);
    auto it = links_.insert(link);
    return std::make_pair(it, links_.count(link.first) == count);
}

template <typename N, typename L>
typename Forest<N, L>::node_iterator
Forest<N, L>::erase(typename Forest<N, L>::node_iterator pos)
{
    const auto& hash = pos->first;
    const auto& node = pos->second;
    assert(node.hash() == hash);

    auto result = pos;
    for (const auto& child : node) // remove links from node to its children
    {
        auto child_link = link_type(child, hash);
        erase(child_link);
    }
    auto node_links = links_.find(hash);
    if (links_.end() == node_links) // Erase node iff no references to it
    {
        result = nodes_.erase(pos);
    }
    return result;
}

template <typename N, typename L>
typename Forest<N, L>::link_iterator
Forest<N, L>::erase(const typename Forest<N, L>::link_type& link)
{
    auto result = links_.cend();
    auto range = links_.equal_range(link.first);
    auto it = std::find(range.first, range.second, link);
    if (it != range.second)
    {
        result = erase(it);
    }
    return result;
}

} // namespace merkle
