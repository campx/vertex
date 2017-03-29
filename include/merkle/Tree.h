#pragma once
#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <vector>

namespace merkle
{

template <typename Container>
bool is_in(const typename Container::value_type& value,
           const Container& container)
{
    return std::find(container.begin(), container.end(), value) ==
           container.end();
}

template <typename Container>
void insert_unique(const typename Container::value_type& value,
                   Container& container)
{
    if (container.end() ==
        std::find(container.begin(), container.end(), value))
    {
        container.push_back(value);
    }
}

template <typename Container>
Container unique_copy(const Container& input)
{
    Container output{};
    output.reserve(input.size());
    std::copy_if(input.begin(), input.end(), std::back_inserter(output),
                 [&output](const auto& value) {
                     return is_in(value, output);
                 });
    return output;
}

template <typename Container>
Container exclude_copy(const typename Container::value_type& exclude,
                       const Container& input)
{
    Container output{};
    output.reserve(input.size());
    std::copy_if(input.begin(), input.end(), std::back_inserter(output),
                 [&exclude](const auto& value) { return value != exclude; });
    return output;
}

template <typename Forest>
class Tree
{
public:
    using forest_type = Forest;
    using forest_pointer = typename std::shared_ptr<Forest>;
    using node_iterator = typename Forest::node_iterator;
    using node_type = typename Forest::node_type;
    using node_container = typename Forest::node_container;
    using link_container = typename Forest::link_container;

    Tree(std::shared_ptr<Forest> forest, node_iterator root)
        : forest_(std::move(forest)), root_(std::move(root))
    {
    }

    const link_container& links() const
    {
        return forest_->links();
    }

    const node_container& nodes() const
    {
        return forest_->nodes();
    }

    node_iterator root() const
    {
        return root_;
    }

    bool operator==(const Tree<Forest>& rhs) const;

    node_iterator insert(node_iterator parent, node_iterator child);
    node_iterator insert(node_iterator parent, const node_type& child);
    node_iterator erase(node_iterator parent, node_iterator child);
    node_iterator update(node_iterator source, const node_type& node);

private:
    std::shared_ptr<Forest> forest_;
    node_iterator root_;

    void root(node_iterator value);

    class Pin
    {
    public:
        using link_type = typename Forest::link_type;
        using key_type = typename Forest::key_type;
        Pin(Forest* const forest, key_type key);
        ~Pin();
        void detach()
        {
            forest_ = nullptr;
        }
        Pin(Pin&& other) noexcept;
        const key_type& key()
        {
            return link_.first;
        }

    private:
        Forest* forest_;
        link_type link_;
    };

    class Mapping
    {
    public:
        using key_type = node_iterator;
        explicit Mapping(std::shared_ptr<Forest> forest)
            : forest_(std::move(forest))
        {
        }
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
    if (forest_)
    {
        forest_->insert(link_);
    }
}

template <typename Forest>
Tree<Forest>::Pin::~Pin()
{
    if (forest_)
    {
        forest_->erase(link_);
    }
}

template <typename Forest>
Tree<Forest>::Pin::Pin(Pin&& other) noexcept : forest_(other.forest_),
                                               link_(std::move(other.link_))
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
void Tree<Forest>::root(typename Tree<Forest>::node_iterator value)
{
    auto last_root(root());
    root_ = value;
    forest_->erase(last_root);
}

template <typename Forest>
typename Tree<Forest>::node_iterator
Tree<Forest>::update(typename Tree<Forest>::node_iterator source,
                     const typename Tree<Forest>::node_type& node)
{
    auto inserted_node = forest_->insert(node);
    auto target = inserted_node.first;
    using node_iterator = typename Forest::node_iterator;
    using key_type = typename Forest::key_type;
    auto pins = std::vector<Pin>{}; // pins prevent deletion of WIP subtree
    auto updated = Mapping(forest_);
    updated.set(source, target);
    auto to_visit = std::queue<std::pair<node_iterator, node_iterator>>{};
    auto enqueue_parents = [&to_visit,
                            this](std::pair<key_type, key_type> link) {
        auto child = nodes().find(link.first);
        auto parent = nodes().find(link.second);
        assert(parent != nodes().end());
        assert(child != nodes().end());
        auto grandparents = links().count(link.second);
        if (child != root() &&
            (grandparents != 0 || (grandparents == 0 && parent == root())))
        {
            to_visit.push(std::make_pair(child, parent));
        }
    };
    auto range = links().equal_range(source->first);
    std::for_each(range.first, range.second, enqueue_parents);
    if (source == root())
    {
        root(target);
    }
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
            if (pins.empty() || target_child->first != pins.back().key())
            {
                pins.emplace_back(Pin(forest_.get(), target_child->first));
            }
            auto children = exclude_copy(source_child->first,
                                         target_parent->second.links());
            insert_unique(target_child->first, children);
            auto node = node_type(target_parent->second.data(), children);
            auto inserted_parent = forest_->insert(node);
            auto range = links().equal_range(source_parent->first);
            if (root() == target_parent)
            {
                root(inserted_parent.first);
            }
            else
            {
                auto remaining = to_visit.size();
                std::for_each(range.first, range.second, enqueue_parents);
                if (to_visit.size() == remaining)
                { // reached a source node without finding target root
                    forest_->erase(inserted_parent.first);
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
bool Tree<Forest>::operator==(const Tree<Forest>& rhs) const
{
    return forest_ == rhs.forest_ && root_ == rhs.root_;
}

template <typename Forest>
typename Tree<Forest>::node_iterator
Tree<Forest>::insert(typename Tree<Forest>::node_iterator parent,
                     typename Tree<Forest>::node_iterator child)
{
    auto children = unique_copy(parent->second.links());
    insert_unique(child->first, children);
    auto node = node_type(parent->second.data(), children);
    return update(parent, node);
}

template <typename Forest>
typename Tree<Forest>::node_iterator
Tree<Forest>::insert(typename Tree<Forest>::node_iterator parent,
                     const typename Tree<Forest>::node_type& child)
{
    auto inserted_node = forest_->insert(child);
    return insert(parent, inserted_node.first);
}

/* Follow links up the tree and speculatively generate new branches. When
    the path terminates, check if the endpoint is root, if not, unlink */
template <typename Forest>
typename Tree<Forest>::node_iterator
Tree<Forest>::erase(typename Tree<Forest>::node_iterator parent,
                    typename Tree<Forest>::node_iterator child)
{
    auto children = exclude_copy(child->first, parent->second.links());
    auto node = node_type(parent->second.data(), children);
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
