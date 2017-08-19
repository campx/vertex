#pragma once
#include <algorithm>
#include <vector>

namespace vertex
{

/** A node in a tree, which has data of type T, and a collection of Key
 * pointers to child nodes.
 * Provides a set-like interface for adding and removing children */
template <typename Key, typename T>
class Vertex
{
public:
    using key_type = Key;
    using container_type = std::vector<key_type>;

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

    /** Return an iterator to the first child Key */
    iterator begin();
    /** Return an iterator to the first child Key */
    const_iterator begin() const;
    /** Return an iterator to the first child Key */
    const_iterator cbegin() const;
    /** Return an iterator to the element following the last child Key */
    iterator end();
    /** Return an iterator to the element following the last child Key */
    const_iterator end() const;
    /** Return an iterator to the element following the last child Key */
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
    std::pair<iterator, bool> insert(const Key& child);

    /** Insert a range of children */
    template <typename InputIt>
    iterator insert(iterator pos, InputIt first, InputIt last);

    /** Find a child vertex */
    iterator find(const Key& child);

    /** Find a child vertex */
    const_iterator find(const Key& child) const;

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

template <typename Key, typename T>
Vertex<Key, T>::Vertex(value_type data) : data_(std::move(data))
{
}

template <typename Key, typename T>
typename Vertex<Key, T>::iterator Vertex<Key, T>::begin()
{
    return children_.begin();
}

template <typename Key, typename T>
typename Vertex<Key, T>::iterator Vertex<Key, T>::end()
{
    return children_.end();
}

template <typename Key, typename T>
typename Vertex<Key, T>::const_iterator Vertex<Key, T>::begin() const
{
    return children_.begin();
}

template <typename Key, typename T>
typename Vertex<Key, T>::const_iterator Vertex<Key, T>::end() const
{
    return children_.end();
}

template <typename Key, typename T>
typename Vertex<Key, T>::const_iterator Vertex<Key, T>::cbegin() const
{
    return children_.cbegin();
}

template <typename Key, typename T>
typename Vertex<Key, T>::const_iterator Vertex<Key, T>::cend() const
{
    return children_.cend();
}

template <typename Key, typename T>
typename Vertex<Key, T>::size_type Vertex<Key, T>::size() const
{
    return children_.size();
}

template <typename Key, typename T>
bool Vertex<Key, T>::empty() const
{
    return children_.empty();
}

template <typename Key, typename T>
void Vertex<Key, T>::clear()
{
    return children_.clear();
}

template <typename Key, typename T>
void Vertex<Key, T>::swap(Vertex& other)
{
    std::swap(children_, other.children_);
    std::swap(data_, other.data_);
}

template <typename Key, typename T>
std::pair<typename Vertex<Key, T>::iterator, bool>
Vertex<Key, T>::insert(const Key& child)
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

template <typename Key, typename T>
template <typename InputIt>
typename Vertex<Key, T>::iterator
Vertex<Key, T>::insert(typename Vertex<Key, T>::iterator pos,
                       InputIt first,
                       InputIt last)
{
    return children_.insert(pos, first, last);
}

template <typename Key, typename T>
typename Vertex<Key, T>::iterator Vertex<Key, T>::find(const Key& child)
{
    auto it = std::find(children_.begin(), children_.end(), child);
    return it;
}

template <typename Key, typename T>
typename Vertex<Key, T>::const_iterator
Vertex<Key, T>::find(const Key& child) const
{
    return find(child);
}

template <typename Key, typename T>
typename Vertex<Key, T>::iterator
Vertex<Key, T>::erase(typename Vertex<Key, T>::const_iterator child)
{
    return children_.erase(child);
}

template <typename Key, typename T>
const typename Vertex<Key, T>::value_type& Vertex<Key, T>::data() const
{
    return data_;
}

template <typename Key, typename T>
void Vertex<Key, T>::data(const typename Vertex<Key, T>::value_type& value)
{
    data_ = value;
}

template <typename Key, typename T>
bool Vertex<Key, T>::operator==(const Vertex<Key, T>& rhs) const
{
    return data_ == rhs.data_ && children_ == rhs.children_;
}

template <typename Key, typename T>
bool Vertex<Key, T>::operator!=(const Vertex<Key, T>& rhs) const
{
    return !(*this == rhs);
}

} // namespace vertex
