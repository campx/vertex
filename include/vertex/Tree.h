#pragma once
#include <algorithm>
#include <map>
#include <memory>
#include <queue>
#include <set>
#include <vector>

namespace vertex
{

template <typename VertexStore>
class Tree
{
public:
    using vertex_iterator = typename VertexStore::iterator;
    using vertex_type = typename VertexStore::mapped_type;
    using key_type = typename VertexStore::key_type;
    using edge_type = std::pair<key_type, key_type>;

    explicit Tree(std::shared_ptr<VertexStore> vertices = nullptr);
    Tree(std::shared_ptr<VertexStore> vertices, vertex_iterator root);
    void root(vertex_iterator value);
    vertex_iterator root() const;
    bool empty() const;
    const std::shared_ptr<VertexStore>& vertices() const;

    bool operator==(const Tree<VertexStore>& rhs) const;

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
    std::shared_ptr<VertexStore> vertices_;
    vertex_iterator root_;

    class Pin
    {

    public:
        using value_type = typename VertexStore::edge_value_type;
        using key_type = typename VertexStore::key_type;
        Pin(VertexStore* const vertices, key_type key);
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
        VertexStore* vertices_;
        value_type edge_;
    };

    class Mapping
    {

    public:
        using key_type = vertex_iterator;
        explicit Mapping(std::shared_ptr<VertexStore> vertices)
            : vertices_(std::move(vertices))
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
        const std::shared_ptr<VertexStore> vertices_;
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

template <typename VertexStore>
Tree<VertexStore>::Pin::Pin(VertexStore* const vertices, key_type key)
    : vertices_(vertices), edge_(value_type(key, key))
{
    if (vertices_)
    {
        vertices_->insert(edge_);
    }
}

template <typename VertexStore>
Tree<VertexStore>::Pin::~Pin()
{
    if (vertices_)
    {
        vertices_->erase(edge_);
    }
}

template <typename VertexStore>
Tree<VertexStore>::Pin::Pin(Pin&& other) noexcept
    : vertices_(other.vertices_),
      edge_(std::move(other.edge_))
{
    other.vertices_ = nullptr;
}

template <typename VertexStore>
bool Tree<VertexStore>::Mapping::Compare::
operator()(vertex_iterator a, vertex_iterator b)
{
    return a->first < b->first;
}

template <typename VertexStore>
Tree<VertexStore>::Tree(std::shared_ptr<VertexStore> vertices)
    : vertices_(std::move(vertices))
{
    if (vertices_ == nullptr)
    {
        vertices_ = std::make_shared<VertexStore>();
    }
    root_ = vertices_->insert(typename VertexStore::vertex_type{}).first;
}

template <typename VertexStore>
Tree<VertexStore>::Tree(std::shared_ptr<VertexStore> vertices,
                        vertex_iterator root)
    : vertices_(std::move(vertices)), root_(std::move(root))
{
}

template <typename VertexStore>
typename Tree<VertexStore>::vertex_iterator Tree<VertexStore>::root() const
{
    return root_;
}

template <typename VertexStore>
bool Tree<VertexStore>::empty() const
{
    return root()->second.edges().empty();
}

template <typename VertexStore>
void Tree<VertexStore>::root(typename Tree<VertexStore>::vertex_iterator value)
{
    auto last_root(root());
    root_ = value;
    vertices_->erase(last_root);
}

template <typename VertexStore>
const std::shared_ptr<VertexStore>& Tree<VertexStore>::vertices() const
{
    return vertices_;
}

template <typename VertexStore>
typename Tree<VertexStore>::vertex_iterator
Tree<VertexStore>::update(typename Tree<VertexStore>::vertex_iterator source,
                          const typename Tree<VertexStore>::vertex_type& value)
{
    auto inserted_vertex = vertices_->insert(value);
    auto target = inserted_vertex.first;
    using vertex_iterator = typename VertexStore::vertex_iterator;
    using key_type = typename VertexStore::key_type;
    auto pins = std::vector<Pin>{}; // pins prevent deletion of WIP subtree
    auto updated = Mapping(vertices_);
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
            auto vertex = target_parent->second;
            vertex.edges(children);
            auto inserted_parent = vertices_->insert(vertex);
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

template <typename VertexStore>
bool Tree<VertexStore>::operator==(const Tree<VertexStore>& rhs) const
{
    return vertices_ == rhs.vertices_ && root_ == rhs.root_;
}

template <typename VertexStore>
typename Tree<VertexStore>::vertex_iterator
Tree<VertexStore>::insert(typename Tree<VertexStore>::vertex_iterator parent,
                          typename Tree<VertexStore>::vertex_iterator child)
{
    auto children = unique_copy(parent->second.edges());
    insert_unique(child->first, children);
    auto vertex = vertex_type(parent->second);
    vertex.edges(children);
    return update(parent, vertex);
}

template <typename VertexStore>
typename Tree<VertexStore>::vertex_iterator
Tree<VertexStore>::insert(typename Tree<VertexStore>::vertex_iterator parent,
                          typename Tree<VertexStore>::edge_type edge)
{
    auto children = unique_copy(parent->second.edges());
    insert_unique(edge, children);
    auto vertex = vertex_type(parent->second);
    vertex.edges(children);
    return update(parent, vertex);
}

template <typename VertexStore>
typename Tree<VertexStore>::vertex_iterator
Tree<VertexStore>::insert(typename Tree<VertexStore>::vertex_iterator parent,
                          const typename Tree<VertexStore>::vertex_type& child)
{
    vertices_->insert(child);
    return insert(parent, child.self_edge());
}

/* Follow edges up the tree and speculatively generate new branches. When
    the path terminates, check if the endpoint is root, if not, unedge */
template <typename VertexStore>
typename Tree<VertexStore>::vertex_iterator
Tree<VertexStore>::erase(typename Tree<VertexStore>::vertex_iterator parent,
                         typename Tree<VertexStore>::vertex_iterator child)
{
    auto children = exclude_copy(child->first, parent->second.edges());
    auto vertex = vertex_type(parent->second);
    vertex.edges(children);
    return update(parent, vertex);
}

template <typename VertexStore>
void Tree<VertexStore>::Mapping::set(
    typename Tree<VertexStore>::vertex_iterator input,
    typename Tree<VertexStore>::vertex_iterator output)
{
    if (input != output)
    {
        map_[input] = output;
    }
}

template <typename VertexStore>
typename Tree<VertexStore>::vertex_iterator Tree<VertexStore>::Mapping::get(
    typename Tree<VertexStore>::vertex_iterator input)

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

} // namespace vertex
