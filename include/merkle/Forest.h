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
     * @return Updated root node, updated parent node */
    std::pair<node_iterator, node_iterator>
    insert(node_iterator root, node_iterator parent, node_iterator child);

    /** Remove the node at the specified position
     * The position must be valid and the node must be an orphan **/
    node_iterator erase(node_iterator pos);

private:
    /** RAII class to prevent deletion of a node within an enclosing scope */
    class Pin
    {
    public:
        Pin(Forest* forest, key_type key)
            : forest_(forest), link_(link_type(key, key))
        {
            if (forest_) forest_->insert(link_);
        }
        ~Pin()
        {
            if (forest_) forest_->erase(link_);
        }
        void detach() { forest_ = nullptr; }
        Pin(Pin&& other)
            : forest_(other.forest_), link_(std::move(other.link_))
        {
            other.forest_ = nullptr;
        }
        const key_type& key() { return link_.first; }

    private:
        Forest* forest_;
        link_type link_;
    };

    class Mapping
    {
    public:
        using key_type = node_iterator;
        Mapping(const NodeStore* const store) : store_(store) {}
        struct Compare
        {
            bool operator()(node_iterator a, node_iterator b)
            {
                return a->first < b->first;
            }
        };
        /** Set the corresponding output for input */
        void set(node_iterator input, node_iterator output);
        /** Get the corresponding output for input */
        node_iterator get(node_iterator input);

    private:
        const NodeStore* const store_;
        std::map<key_type, key_type, Compare> map_;
    };

    /** Insert a link into the graph
     * Parent and child nodes must exist */
    std::pair<link_iterator, bool> insert(const link_type& link);

    /** Get an iterator to the link matching the argument */
    link_iterator find(const link_type& link);

    /** Remove the given link */
    link_iterator erase(const link_type& link);

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
    for (const auto& child : node)
    { // add a link from node to each of its children
        insert(link_type(child, node.hash()));
    }
    return result;
}

/* Follow links up the tree and speculatively generate new branches. When
    the path terminates, check if the endpoint is root, if not, unlink */
template <typename N, typename L>
typename std::pair<typename Forest<N, L>::node_iterator,
                   typename Forest<N, L>::node_iterator>
Forest<N, L>::insert(typename Forest<N, L>::node_iterator root,
                     typename Forest<N, L>::node_iterator parent,
                     typename Forest<N, L>::node_iterator child)
{
    using node_iterator = Forest<N, L>::node_iterator;
    std::vector<Pin> pins; // pins prevent deletion of subtree
    pins.emplace_back(Pin(this, child->first));
    auto result = std::make_pair(root, parent);
    Mapping updated(&nodes()); // map from old tree to new tree
    std::queue<std::pair<node_iterator, node_iterator>> to_visit;
    to_visit.push(std::make_pair(child, parent));
    while (!to_visit.empty())
    {
        auto next_pair = to_visit.front();
        to_visit.pop();
        const auto source_child = next_pair.first;
        const auto source_parent = next_pair.second;
        auto target_parent = updated.get(source_parent);
        auto target_child = updated.get(source_child);
        if (target_parent != nodes().end())
        {
            if (target_child->first != pins.back().key())
                pins.emplace_back(Pin(this, target_child->first));
            auto node = target_parent->second;
            node.erase(source_child->first);
            node.insert(target_child->first);
            auto inserted_parent = insert(node);
            auto range = links().equal_range(source_parent->second.hash());
            if (source_child == child) result.second = inserted_parent.first;
            if (root == target_parent)
                result.first = inserted_parent.first;
            else
            {
                for (auto it = range.first; it != range.second; ++it)
                { // enqueue grandparents
                    auto grandparent = nodes().find(it->second);
                    assert(grandparent != nodes().end());
                    to_visit.push(std::make_pair(source_parent, grandparent));
                }
                if (range.first == range.second && inserted_parent.second)
                { // reached a source node without finding target root
                    erase(inserted_parent.first);
                }
                else
                {
                    target_parent = inserted_parent.first;
                }
                updated.set(source_parent, target_parent);
            }
        }
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
                for (const auto& grandchild : node->second)
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
        for (const auto& child : node)
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

template <typename N, typename L>
void Forest<N, L>::Mapping::set(typename Forest<N, L>::node_iterator input,
                                typename Forest<N, L>::node_iterator output)
{
    if (input != output)
    {
        map_[input] = output;
    }
}

template <typename N, typename L>
typename Forest<N, L>::node_iterator
Forest<N, L>::Mapping::get(typename Forest<N, L>::node_iterator input)

{
    auto output = input;
    auto output_it = map_.find(input);
    if (output_it != map_.end())
    {
        output = output_it->second;
        output = store_->find(output->first);
    }
    return output;
}

} // namespace merkle
