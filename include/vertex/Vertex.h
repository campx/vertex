#pragma once
#include <algorithm>
#include <vector>

namespace vertex
{

/** A node in a tree, which has data of type T, and a collection of Edge
 * pointers to child nodes.
 * Provides a set-like interface for adding and removing children */
template <typename Edge, typename T>
class Vertex
{
public:
    using edge_type = Edge;
    using container_type = std::vector<edge_type>;

    using value_type = T;
    using size_type = typename container_type::size_type;
    using difference_type = typename container_type::difference_type;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using pointer = typename container_type::pointer;
    using const_pointer = typename container_type::const_pointer;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using reverse_iterator = typename container_type::reverse_iterator;
    using const_reverse_iterator =
        typename container_type::const_reverse_iterator;

    /** Create a Vertex with the given data value */
    explicit Vertex(value_type data = value_type());

    Vertex(const Vertex&) = default;
    Vertex(Vertex&&) = default;
    Vertex& operator=(const Vertex&) = default;
    Vertex& operator=(Vertex&&) = default;

    /** Return an iterator to the first child Edge */
    iterator begin();
    /** Return an iterator to the first child Edge */
    const_iterator begin() const;
    /** Return an iterator to the first child Edge */
    const_iterator cbegin() const;
    /** Return an iterator to the element following the last child Edge */
    iterator end();
    /** Return an iterator to the element following the last child Edge */
    const_iterator end() const;
    /** Return an iterator to the element following the last child Edge */
    const_iterator cend() const;

    /** Get the number of children */
    size_type size() const;

    /** Check whether the vertex has any children */
    bool empty() const;

    /** Remove all children from the Vertex */
    void clear();

    /** Exchanges the contents of the Vertex with those of other */
    void swap(Vertex& other);

    /** Insert a child vertex */
    std::pair<iterator, bool> insert(const Edge& child);

    /** Insert a range of children */
    template <typename InputIt>
    iterator insert(iterator pos, InputIt first, InputIt last);

    /** Find a child vertex */
    iterator find(const Edge& child);

    /** Find a child vertex */
    const_iterator find(const Edge& child) const;

    /** Remove a child vertex */
    iterator erase(const_iterator child);

    /** Get the data associated with this vertex */
    const value_type& data() const;

    /** Set the data associated with this vertex */
    void data(const value_type& value);

    /** Compare the contents and children of a Vertex against another */
    bool operator==(const Vertex& rhs) const;

    /** Compare the contents and children of a Vertex against another */
    bool operator!=(const Vertex& rhs) const;

private:
    value_type data_;
    container_type children_;
};

template <typename Edge, typename T>
Vertex<Edge, T>::Vertex(value_type data) : data_(std::move(data))
{
}

template <typename Edge, typename T>
typename Vertex<Edge, T>::iterator Vertex<Edge, T>::begin()
{
    return children_.begin();
}

template <typename Edge, typename T>
typename Vertex<Edge, T>::iterator Vertex<Edge, T>::end()
{
    return children_.end();
}

template <typename Edge, typename T>
typename Vertex<Edge, T>::const_iterator Vertex<Edge, T>::begin() const
{
    return children_.begin();
}

template <typename Edge, typename T>
typename Vertex<Edge, T>::const_iterator Vertex<Edge, T>::end() const
{
    return children_.end();
}

template <typename Edge, typename T>
typename Vertex<Edge, T>::const_iterator Vertex<Edge, T>::cbegin() const
{
    return children_.cbegin();
}

template <typename Edge, typename T>
typename Vertex<Edge, T>::const_iterator Vertex<Edge, T>::cend() const
{
    return children_.cend();
}

template <typename Edge, typename T>
typename Vertex<Edge, T>::size_type Vertex<Edge, T>::size() const
{
    return children_.size();
}

template <typename Edge, typename T>
bool Vertex<Edge, T>::empty() const
{
    return children_.empty();
}

template <typename Edge, typename T>
void Vertex<Edge, T>::clear()
{
    return children_.clear();
}

template <typename Edge, typename T>
void Vertex<Edge, T>::swap(Vertex& other)
{
    std::swap(children_, other.children_);
    std::swap(data_, other.data_);
}

template <typename Edge, typename T>
std::pair<typename Vertex<Edge, T>::iterator, bool>
Vertex<Edge, T>::insert(const Edge& child)
{
    auto it = find(child);
    auto do_insert = it == end();
    if (do_insert)
    {
        it = children_.insert(children_.end(), child);
    }
    auto result = std::make_pair(it, do_insert);
    return result;
}

template <typename Edge, typename T>
template <typename InputIt>
typename Vertex<Edge, T>::iterator
Vertex<Edge, T>::insert(typename Vertex<Edge, T>::iterator pos,
                        InputIt first,
                        InputIt last)
{
    return children_.insert(pos, first, last);
}

template <typename Edge, typename T>
typename Vertex<Edge, T>::iterator Vertex<Edge, T>::find(const Edge& child)
{
    auto it = std::find(children_.begin(), children_.end(), child);
    return it;
}

template <typename Edge, typename T>
typename Vertex<Edge, T>::const_iterator
Vertex<Edge, T>::find(const Edge& child) const
{
    return find(child);
}

template <typename Edge, typename T>
typename Vertex<Edge, T>::iterator
Vertex<Edge, T>::erase(typename Vertex<Edge, T>::const_iterator child)
{
    return children_.erase(child);
}

template <typename Edge, typename T>
const typename Vertex<Edge, T>::value_type& Vertex<Edge, T>::data() const
{
    return data_;
}

template <typename Edge, typename T>
void Vertex<Edge, T>::data(const typename Vertex<Edge, T>::value_type& value)
{
    data_ = value;
}

template <typename Edge, typename T>
bool Vertex<Edge, T>::operator==(const Vertex<Edge, T>& rhs) const
{
    return data_ == rhs.data_ && children_ == rhs.children_;
}

template <typename Edge, typename T>
bool Vertex<Edge, T>::operator!=(const Vertex<Edge, T>& rhs) const
{
    return !(*this == rhs);
}

} // namespace vertex
