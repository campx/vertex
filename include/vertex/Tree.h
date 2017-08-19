#pragma once
#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <vector>

namespace vertex
{

template <typename Forest>
class Tree
{
public:
    using forest_type = Forest;
    using forest_pointer = typename std::shared_ptr<Forest>;
    using vertex_iterator = typename Forest::vertex_iterator;
    using vertex_type = typename Forest::vertex_type;
    using edge_type = typename Forest::edge_type;
    using vertex_container = typename Forest::vertex_container;
    using edge_container = typename Forest::edge_container;

    explicit Tree(std::shared_ptr<Forest> forest = nullptr);
    Tree(std::shared_ptr<Forest> forest, vertex_iterator root);
    const edge_container& edges() const;
    const vertex_container& vertices() const;
    void root(vertex_iterator value);
    vertex_iterator root() const;
    bool empty() const;
    const std::shared_ptr<Forest>& forest() const;

    bool operator==(const Tree<Forest>& rhs) const;

    /** Insert a vertex  iterator insert(const vertex_type& vertex);  */
    /** Insert a edge  iterator insert(std::pair<Edge, Edge>);  */
    /** Find a vertex    iterator find(Edge); */
    /** Erase a edge   iterator erase(std::pair<Edge, Edge>); */
    /** Erase a vertex   iterator erase(Edge); */

    vertex_iterator insert(vertex_iterator parent, vertex_iterator child);
    vertex_iterator insert(vertex_iterator parent, edge_type edge);
    vertex_iterator insert(vertex_iterator parent, const vertex_type& child);
    vertex_iterator erase(vertex_iterator parent, vertex_iterator child);
    vertex_iterator update(vertex_iterator source, const vertex_type& value);

private:
    std::shared_ptr<Forest> forest_;
    vertex_iterator root_;

    class Pin
    {

    public:
        using value_type = typename Forest::edge_value_type;
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
            return edge_.first;
        }

    private:
        Forest* forest_;
        value_type edge_;
    };

    class Mapping
    {

    public:
        using key_type = vertex_iterator;
        explicit Mapping(std::shared_ptr<Forest> forest)
            : forest_(std::move(forest))
        {
        }
        struct Compare
        {
            bool operator()(vertex_iterator a, vertex_iterator b);
        };
        /** Set the corresponding output for input */
        void set(vertex_iterator input, vertex_iterator output);
        /** Get the corresponding output for input */
        vertex_iterator get(vertex_iterator input);

    private:
        const std::shared_ptr<Forest> forest_;
        std::map<key_type, key_type, Compare> map_;
    };
};

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
Tree<Forest>::Pin::Pin(Forest* const forest, key_type key)
    : forest_(forest), edge_(value_type(key, key))
{
    if (forest_)
    {
        forest_->insert(edge_);
    }
}

template <typename Forest>
Tree<Forest>::Pin::~Pin()
{
    if (forest_)
    {
        forest_->erase(edge_);
    }
}

template <typename Forest>
Tree<Forest>::Pin::Pin(Pin&& other) noexcept : forest_(other.forest_),
                                               edge_(std::move(other.edge_))
{
    other.forest_ = nullptr;
}

template <typename Forest>
bool Tree<Forest>::Mapping::Compare::
operator()(vertex_iterator a, vertex_iterator b)
{
    return a->first < b->first;
}

template <typename Forest>
Tree<Forest>::Tree(std::shared_ptr<Forest> forest) : forest_(std::move(forest))
{
    if (forest_ == nullptr)
    {
        forest_ = std::make_shared<Forest>();
    }
    root_ = forest_->insert(typename Forest::vertex_type{}).first;
}

template <typename Forest>
Tree<Forest>::Tree(std::shared_ptr<Forest> forest, vertex_iterator root)
    : forest_(std::move(forest)), root_(std::move(root))
{
}

template <typename Forest>
const typename Tree<Forest>::edge_container& Tree<Forest>::edges() const
{
    return forest_->edges();
}

template <typename Forest>
const typename Tree<Forest>::vertex_container& Tree<Forest>::vertices() const
{
    return forest_->vertices();
}

template <typename Forest>
typename Tree<Forest>::vertex_iterator Tree<Forest>::root() const
{
    return root_;
}

template <typename Forest>
bool Tree<Forest>::empty() const
{
    return root()->second.edges().empty();
}

template <typename Forest>
void Tree<Forest>::root(typename Tree<Forest>::vertex_iterator value)
{
    auto last_root(root());
    root_ = value;
    forest_->erase(last_root);
}

template <typename Forest>
const std::shared_ptr<Forest>& Tree<Forest>::forest() const
{
    return forest_;
}

template <typename Forest>
typename Tree<Forest>::vertex_iterator
Tree<Forest>::update(typename Tree<Forest>::vertex_iterator source,
                     const typename Tree<Forest>::vertex_type& value)
{
    auto inserted_vertex = forest_->insert(value);
    auto target = inserted_vertex.first;
    using vertex_iterator = typename Forest::vertex_iterator;
    using key_type = typename Forest::key_type;
    auto pins = std::vector<Pin>{}; // pins prevent deletion of WIP subtree
    auto updated = Mapping(forest_);
    updated.set(source, target);
    auto to_visit = std::queue<std::pair<vertex_iterator, vertex_iterator>>{};
    auto enqueue_parents = [&to_visit, &value,
                            this](std::pair<key_type, key_type> edge) {
        auto child = vertices().find(edge.first);
        auto parent = vertices().find(edge.second);
        assert(parent != vertices().end());
        assert(child != vertices().end());
        auto it =
            std::find(value.edges().begin(), value.edges().end(), edge.second);
        auto grandparents = edges().count(edge.second);
        if (child != root() && parent->first != value.self_edge() &&
            it == value.edges().end() &&
            (grandparents != 0 || (grandparents == 0 && parent == root())))
        {
            to_visit.push(std::make_pair(child, parent));
        }
    };
    auto range = edges().equal_range(source->first);
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
        if (target_parent != vertices().end() &&
            target_child != vertices().end())
        {
            if (pins.empty() || target_child->first != pins.back().key())
            {
                pins.emplace_back(Pin(forest_.get(), target_child->first));
            }
            auto children = exclude_copy(source_child->first,
                                         target_parent->second.edges());
            insert_unique(target_child->first, children);
            auto vertex = target_parent->second;
            vertex.edges(children);
            auto inserted_parent = forest_->insert(vertex);
            auto range = edges().equal_range(source_parent->first);
            if (root() == target_parent)
            {
                root(inserted_parent.first);
            }
            else
            {
                auto remaining = to_visit.size();
                std::for_each(range.first, range.second, enqueue_parents);
                if (to_visit.size() == remaining)
                { // reached a source vertex without finding target root
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
typename Tree<Forest>::vertex_iterator
Tree<Forest>::insert(typename Tree<Forest>::vertex_iterator parent,
                     typename Tree<Forest>::vertex_iterator child)
{
    auto children = unique_copy(parent->second.edges());
    insert_unique(child->first, children);
    auto vertex = vertex_type(parent->second);
    vertex.edges(children);
    return update(parent, vertex);
}

template <typename Forest>
typename Tree<Forest>::vertex_iterator
Tree<Forest>::insert(typename Tree<Forest>::vertex_iterator parent,
                     typename Tree<Forest>::edge_type edge)
{
    auto children = unique_copy(parent->second.edges());
    insert_unique(edge, children);
    auto vertex = vertex_type(parent->second);
    vertex.edges(children);
    return update(parent, vertex);
}

template <typename Forest>
typename Tree<Forest>::vertex_iterator
Tree<Forest>::insert(typename Tree<Forest>::vertex_iterator parent,
                     const typename Tree<Forest>::vertex_type& child)
{
    forest_->insert(child);
    return insert(parent, child.self_edge());
}

/* Follow edges up the tree and speculatively generate new branches. When
    the path terminates, check if the endpoint is root, if not, unedge */
template <typename Forest>
typename Tree<Forest>::vertex_iterator
Tree<Forest>::erase(typename Tree<Forest>::vertex_iterator parent,
                    typename Tree<Forest>::vertex_iterator child)
{
    auto children = exclude_copy(child->first, parent->second.edges());
    auto vertex = vertex_type(parent->second);
    vertex.edges(children);
    return update(parent, vertex);
}

template <typename Forest>
void Tree<Forest>::Mapping::set(typename Tree<Forest>::vertex_iterator input,
                                typename Tree<Forest>::vertex_iterator output)
{
    if (input != output)
    {
        map_[input] = output;
    }
}

template <typename Forest>
typename Tree<Forest>::vertex_iterator
Tree<Forest>::Mapping::get(typename Tree<Forest>::vertex_iterator input)

{
    auto output = input;
    auto output_it = map_.find(input);
    if (output_it != map_.end())
    {
        output = output_it->second;
        output = forest_->vertices().find(output->first);
    }
    return output;
}

} // namespace vertex
