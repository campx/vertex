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

    const Forest& forest() const { return forest_; }
    node_iterator root() const { return root_; }

    node_iterator insert(node_iterator parent, const node_type& child);
    node_iterator erase(node_iterator parent, node_iterator child);
    node_iterator update(node_iterator source, const node_type& node);

private:
    const std::shared_ptr<Forest> forest_;
    node_iterator root_;

    class Pin
    {
    public:
        using link_type = typename Forest::link_type;
        using key_type = typename Forest::key_type;
        Pin(const Forest* const forest, key_type key);
        ~Pin();
        void detach() { forest_ = nullptr; }
        Pin(Pin&& other);
        const key_type& key() { return link_.first; }
    private:
        const Forest* forest_;
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
Tree<Forest>::Pin::Pin(const Forest* const forest, key_type key)
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
    auto target = forest_->insert(node);
    using node_iterator = typename Forest::node_iterator;
    using key_type = typename Forest::key_type;
    auto pins = std::vector<Pin>{}; // pins prevent deletion of WIP subtree
    auto updated = Mapping(forest());
    updated.set(source, target);
    auto to_visit = std::queue<std::pair<node_iterator, node_iterator>>{};
    auto enqueue_parents = [&to_visit,
                            this](std::pair<key_type, key_type> link) {
        auto child = forest_->nodes().find(link.first);
        auto parent = forest_->nodes().find(link.second);
        assert(parent != forest_->nodes().end());
        assert(child != forest_->nodes().end());
        auto grandparents = forest_->links().count(link.second);
        if (grandparents != 0 || (grandparents == 0 && parent == root_))
            to_visit.push(std::make_pair(child, parent));
    };
    auto range = forest_->links().equal_range(source->first);
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
        if (target_parent != forest_->nodes().end())
        {
            if (pins.empty() || target_child->first != pins.back().key())
                pins.emplace_back(Pin(this, target_child->first));
            auto children = std::set<key_type>(target_parent->second.begin(),
                                               target_parent->second.end());
            children.erase(source_child->first);
            children.insert(target_child->first);
            auto node = node_type(children.begin(), children.end(),
                                  target_parent->second.data());
            auto inserted_parent = forest_->insert(node);
            auto range =
                forest_->links().equal_range(source_parent->second.hash());
            if (root() == target_parent)
                root_ = inserted_parent.first;
            else
            {
                std::for_each(range.first, range.second, enqueue_parents);
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
    return target;
}

} // namespace merkle
