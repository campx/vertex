#pragma once
#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <vector>

namespace objex
{

template <typename NodeMap>
class Tree
{
public:
    using objex_iterator = typename NodeMap::iterator;
    using objex_type = typename NodeMap::mapped_type;
    using key_type = typename NodeMap::key_type;
    using edge_type = std::pair<key_type, key_type>;

    explicit Tree(std::shared_ptr<NodeMap> vertices = nullptr);
    Tree(std::shared_ptr<NodeMap> vertices, objex_iterator root);
    void root(objex_iterator value);
    objex_iterator root() const;
    bool empty() const;
    const std::shared_ptr<NodeMap>& vertices() const;

    bool operator==(const Tree<NodeMap>& rhs) const;

    /** Insert a objex  iterator insert(const objex_type& objex);  */
    /** Insert a edge  iterator insert(std::pair<Edge, Edge>);  */
    /** Find a objex    iterator find(Edge); */
    /** Erase a edge   iterator erase(std::pair<Edge, Edge>); */
    /** Erase a objex   iterator erase(Edge); */

    objex_iterator insert(objex_iterator parent, objex_iterator child);
    objex_iterator insert(objex_iterator parent, edge_type edge);
    objex_iterator insert(objex_iterator parent, const objex_type& child);
    objex_iterator erase(objex_iterator parent, objex_iterator child);
    objex_iterator update(objex_iterator source, const objex_type& value);

private:
    std::shared_ptr<NodeMap> vertices_;
    objex_iterator root_;

    class Pin
    {

    public:
        using value_type = typename NodeMap::edge_value_type;
        using key_type = typename NodeMap::key_type;
        Pin(NodeMap* const vertices, key_type key);
        ~Pin();
        void detach()
        {
            vertices_ = nullptr;
        }
        Pin(Pin&& other) noexcept;
        const key_type& key()
        {
            return edge_.first;
        }

    private:
        NodeMap* vertices_;
        value_type edge_;
    };

    class Mapping
    {

    public:
        using key_type = objex_iterator;
        explicit Mapping(std::shared_ptr<NodeMap> vertices)
            : vertices_(std::move(vertices))
        {
        }
        struct Compare
        {
            bool operator()(objex_iterator a, objex_iterator b);
        };
        /** Set the corresponding output for input */
        void set(objex_iterator input, objex_iterator output);
        /** Get the corresponding output for input */
        objex_iterator get(objex_iterator input);

    private:
        const std::shared_ptr<NodeMap> vertices_;
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
    output.reserve(input.length());
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
    output.reserve(input.length());
    std::copy_if(input.begin(), input.end(), std::back_inserter(output),
                 [&exclude](const auto& value) { return value != exclude; });
    return output;
}

template <typename NodeMap>
Tree<NodeMap>::Pin::Pin(NodeMap* const vertices, key_type key)
    : vertices_(vertices), edge_(value_type(key, key))
{
    if (vertices_)
    {
        vertices_->insert(edge_);
    }
}

template <typename NodeMap>
Tree<NodeMap>::Pin::~Pin()
{
    if (vertices_)
    {
        vertices_->erase(edge_);
    }
}

template <typename NodeMap>
Tree<NodeMap>::Pin::Pin(Pin&& other) noexcept : vertices_(other.vertices_),
                                                edge_(std::move(other.edge_))
{
    other.vertices_ = nullptr;
}

template <typename NodeMap>
bool Tree<NodeMap>::Mapping::Compare::
operator()(objex_iterator a, objex_iterator b)
{
    return a->first < b->first;
}

template <typename NodeMap>
Tree<NodeMap>::Tree(std::shared_ptr<NodeMap> vertices)
    : vertices_(std::move(vertices))
{
    if (vertices_ == nullptr)
    {
        vertices_ = std::make_shared<NodeMap>();
    }
    root_ = vertices_->insert(typename NodeMap::objex_type{}).first;
}

template <typename NodeMap>
Tree<NodeMap>::Tree(std::shared_ptr<NodeMap> vertices, objex_iterator root)
    : vertices_(std::move(vertices)), root_(std::move(root))
{
}

template <typename NodeMap>
typename Tree<NodeMap>::objex_iterator Tree<NodeMap>::root() const
{
    return root_;
}

template <typename NodeMap>
bool Tree<NodeMap>::empty() const
{
    return root()->second.edges().empty();
}

template <typename NodeMap>
void Tree<NodeMap>::root(typename Tree<NodeMap>::objex_iterator value)
{
    auto last_root(root());
    root_ = value;
    vertices_->erase(last_root);
}

template <typename NodeMap>
const std::shared_ptr<NodeMap>& Tree<NodeMap>::vertices() const
{
    return vertices_;
}

template <typename NodeMap>
typename Tree<NodeMap>::objex_iterator
Tree<NodeMap>::update(typename Tree<NodeMap>::objex_iterator source,
                      const typename Tree<NodeMap>::objex_type& value)
{
    auto inserted_objex = vertices_->insert(value);
    auto target = inserted_objex.first;
    using objex_iterator = typename NodeMap::objex_iterator;
    using key_type = typename NodeMap::key_type;
    auto pins = std::vector<Pin>{}; // pins prevent deletion of WIP subtree
    auto updated = Mapping(vertices_);
    updated.set(source, target);
    auto to_visit = std::queue<std::pair<objex_iterator, objex_iterator>>{};
    auto enqueue_parents = [&to_visit, &value,
                            this](std::pair<key_type, key_type> edge) {
        auto child = vertices().find(edge.first);
        auto parent = vertices().find(edge.second);
        assert(parent != vertices().end());
        assert(child != vertices().end());
        auto it =
            std::find(value.edges().begin(), value.edges().end(), edge.second);
        auto grandparents = vertices().count(edge.second);
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
                pins.emplace_back(Pin(vertices_.get(), target_child->first));
            }
            auto children = exclude_copy(source_child->first,
                                         target_parent->second.edges());
            insert_unique(target_child->first, children);
            auto objex = target_parent->second;
            objex.edges(children);
            auto inserted_parent = vertices_->insert(objex);
            auto range = edges().equal_range(source_parent->first);
            if (root() == target_parent)
            {
                root(inserted_parent.first);
            }
            else
            {
                auto remaining = to_visit.length();
                std::for_each(range.first, range.second, enqueue_parents);
                if (to_visit.length() == remaining)
                { // reached a source objex without finding target root
                    vertices_->erase(inserted_parent.first);
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

template <typename NodeMap>
bool Tree<NodeMap>::operator==(const Tree<NodeMap>& rhs) const
{
    return vertices_ == rhs.vertices_ && root_ == rhs.root_;
}

template <typename NodeMap>
typename Tree<NodeMap>::objex_iterator
Tree<NodeMap>::insert(typename Tree<NodeMap>::objex_iterator parent,
                      typename Tree<NodeMap>::objex_iterator child)
{
    auto children = unique_copy(parent->second.edges());
    insert_unique(child->first, children);
    auto objex = objex_type(parent->second);
    objex.edges(children);
    return update(parent, objex);
}

template <typename NodeMap>
typename Tree<NodeMap>::objex_iterator
Tree<NodeMap>::insert(typename Tree<NodeMap>::objex_iterator parent,
                      typename Tree<NodeMap>::edge_type edge)
{
    auto children = unique_copy(parent->second.edges());
    insert_unique(edge, children);
    auto objex = objex_type(parent->second);
    objex.edges(children);
    return update(parent, objex);
}

template <typename NodeMap>
typename Tree<NodeMap>::objex_iterator
Tree<NodeMap>::insert(typename Tree<NodeMap>::objex_iterator parent,
                      const typename Tree<NodeMap>::objex_type& child)
{
    vertices_->insert(child);
    return insert(parent, child.self_edge());
}

/* Follow edges up the tree and speculatively generate new branches. When
    the path terminates, check if the endpoint is root, if not, unedge */
template <typename NodeMap>
typename Tree<NodeMap>::objex_iterator
Tree<NodeMap>::erase(typename Tree<NodeMap>::objex_iterator parent,
                     typename Tree<NodeMap>::objex_iterator child)
{
    auto children = exclude_copy(child->first, parent->second.edges());
    auto objex = objex_type(parent->second);
    objex.edges(children);
    return update(parent, objex);
}

template <typename NodeMap>
void Tree<NodeMap>::Mapping::set(typename Tree<NodeMap>::objex_iterator input,
                                 typename Tree<NodeMap>::objex_iterator output)
{
    if (input != output)
    {
        map_[input] = output;
    }
}

template <typename NodeMap>
typename Tree<NodeMap>::objex_iterator
Tree<NodeMap>::Mapping::get(typename Tree<NodeMap>::objex_iterator input)

{
    auto output = input;
    auto output_it = map_.find(input);
    if (output_it != map_.end())
    {
        output = output_it->second;
        output = vertices_->vertices().find(output->first);
    }
    return output;
}

} // namespace objex
