#pragma once
#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <vector>

namespace merkle
{

template <typename Forest>
class Tree
{
public:
    using node_iterator = typename Forest::node_iterator;
    using node_type = typename Forest::node_type;

    Tree(std::shared_ptr<Forest> forest, node_iterator root)
        : forest_(std::move(forest)), root_(root)
    {
    }

    const std::shared_ptr<Forest>& forest() const { return forest_; }
    node_iterator root() const { return root_; }

    node_iterator insert(node_iterator parent, const node_type& child);
    node_iterator erase(node_iterator parent, node_iterator child);
    node_iterator update(node_iterator source, const node_type& node);

private:
    std::shared_ptr<Forest> forest_;
    node_iterator root_;

    class Pin
    {
    public:
        using link_type = typename Forest::link_type;
        using key_type = typename Forest::key_type;
        Pin(Forest* const forest, key_type key);
        ~Pin();
        void detach() { forest_ = nullptr; }
        Pin(Pin&& other);
        const key_type& key() { return link_.first; }
    private:
        Forest* forest_;
        link_type link_;
    };

    class Mapping
    {
    public:
        using key_type = node_iterator;
        Mapping(std::shared_ptr<Forest> forest) : forest_(forest) {}
        struct Compare
        {
            bool operator()(node_iterator a, node_iterator b);
        };
        /** Set the corresponding output for input */
        void set(node_iterator input, node_iterator output);
        /** Get the corresponding output for input */
        node_iterator get(node_iterator input);

    private:
        const std::shared_ptr<Forest> forest_;
        std::map<key_type, key_type, Compare> map_;
    };
};

template <typename Forest>
Tree<Forest>::Pin::Pin(Forest* const forest, key_type key)
    : forest_(forest), link_(link_type(key, key))
{
    if (forest_) forest_->insert(link_);
}

template <typename Forest>
Tree<Forest>::Pin::~Pin()
{
    if (forest_) forest_->erase(link_);
}

template <typename Forest>
Tree<Forest>::Pin::Pin(Pin&& other)
    : forest_(other.forest_), link_(std::move(other.link_))
{
    other.forest_ = nullptr;
}

template <typename Forest>
bool Tree<Forest>::Mapping::Compare::
operator()(node_iterator a, node_iterator b)
{
    return a->first < b->first;
}

template <typename Forest>
typename Tree<Forest>::node_iterator
Tree<Forest>::update(typename Tree<Forest>::node_iterator source,
                     const typename Tree<Forest>::node_type& node)
{
    auto target = forest()->insert(node).first;
    using node_iterator = typename Forest::node_iterator;
    using key_type = typename Forest::key_type;
    auto pins = std::vector<Pin>{}; // pins prevent deletion of WIP subtree
    auto updated = Mapping(forest());
    updated.set(source, target);
    auto to_visit = std::queue<std::pair<node_iterator, node_iterator>>{};
    auto enqueue_parents = [&to_visit,
                            this](std::pair<key_type, key_type> link) {
        auto child = forest()->nodes().find(link.first);
        auto parent = forest()->nodes().find(link.second);
        assert(parent != forest()->nodes().end());
        assert(child != forest()->nodes().end());
        auto grandparents = forest()->links().count(link.second);
        if (grandparents != 0 || (grandparents == 0 && parent == root_))
            to_visit.push(std::make_pair(child, parent));
    };
    auto range = forest()->links().equal_range(source->first);
    std::for_each(range.first, range.second, enqueue_parents);
    if (source == root() && to_visit.empty()) root_ = target;
    while (!to_visit.empty())
    {
        auto next_pair = to_visit.front();
        to_visit.pop();
        const auto source_child = next_pair.first;
        const auto source_parent = next_pair.second;
        auto target_parent = updated.get(source_parent);
        auto target_child = updated.get(source_child);
        if (target_parent != forest()->nodes().end())
        {
            if (pins.empty() || target_child->first != pins.back().key())
                pins.emplace_back(Pin(forest().get(), target_child->first));
            auto children = std::set<key_type>(target_parent->second.begin(),
                                               target_parent->second.end());
            children.erase(source_child->first);
            children.insert(target_child->first);
            auto node = node_type(children.begin(), children.end(),
                                  target_parent->second.data());
            auto inserted_parent = forest()->insert(node);
            auto range =
                forest()->links().equal_range(source_parent->second.hash());
            if (root() == target_parent)
                root_ = inserted_parent.first;
            else
            {
                std::for_each(range.first, range.second, enqueue_parents);
                if (range.first == range.second && inserted_parent.second)
                { // reached a source node without finding target root
                    forest()->erase(inserted_parent.first);
                }
                else
                {
                    target_parent = inserted_parent.first;
                }
                updated.set(source_parent, target_parent);
            }
        }
    }
    return target;
}

template <typename Forest>
typename Tree<Forest>::node_iterator
Tree<Forest>::insert(typename Tree<Forest>::node_iterator parent,
                     const typename Tree<Forest>::node_type& child)
{
    using key_type = typename Forest::key_type;
    forest()->insert(child);
    auto children =
        std::set<key_type>(parent->second.begin(), parent->second.end());
    children.insert(child.hash());
    auto node =
        node_type(children.begin(), children.end(), parent->second.data());
    return update(parent, node);
}

/* Follow links up the tree and speculatively generate new branches. When
    the path terminates, check if the endpoint is root, if not, unlink */
template <typename Forest>
typename Tree<Forest>::node_iterator
Tree<Forest>::erase(typename Tree<Forest>::node_iterator parent,
                    typename Tree<Forest>::node_iterator child)
{
    using key_type = typename Forest::key_type;
    auto children = std::vector<key_type>{};
    std::copy_if(parent->second.begin(), parent->second.end(),
                 std::back_inserter(children),
                 [&child](const key_type& a) { return a != child->first; });
    auto node =
        node_type(children.begin(), children.end(), parent->second.data());
    return update(parent, node);
}

template <typename Forest>
void Tree<Forest>::Mapping::set(typename Tree<Forest>::node_iterator input,
                                typename Tree<Forest>::node_iterator output)
{
    if (input != output)
    {
        map_[input] = output;
    }
}

template <typename Forest>
typename Tree<Forest>::node_iterator
Tree<Forest>::Mapping::get(typename Tree<Forest>::node_iterator input)

{
    auto output = input;
    auto output_it = map_.find(input);
    if (output_it != map_.end())
    {
        output = output_it->second;
        output = forest_->nodes().find(output->first);
    }
    return output;
}
} // namespace merkle