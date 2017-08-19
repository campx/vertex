#pragma once
#include <memory>

namespace vertex
{

/** An iterator which cannot be incremented or decremented and acts simply as a
 * handle for when those operations are not meaningful */
template <typename T>
class TrivialIterator
{
public:
    struct trivial_iterator_tag
    {
    };
    using difference_type = void;
    using iterator_category = trivial_iterator_tag;
    using self_type = TrivialIterator<T>;
    using value_type = typename Iterator::value_type;
    using reference = const value_type&;
    using pointer = const value_type&;

    TrivialIterator() = default;
    TrivialIterator(T it);
    TrivialIterator(const TrivialIterator&) = default;
    TrivialIterator& operator=(const TrivialIterator&) = default;
    TrivialIterator(TrivialIterator&&) = default;
    TrivialIterator& operator=(TrivialIterator&&) = default;

    reference operator*();
    pointer operator->();

    bool operator==(const self_type& rhs) const;
    bool operator!=(const self_type& rhs) const;

private:
    T data_;
};

template <typename T>
TrivialIterator<T>::TrivialIterator(T it) : data_(std::move(it))
{
}

template <typename T>
typename TrivialIterator<T>::reference TrivialIterator<T>::operator*()
{
    return *data_;
}

template <typename T>
typename TrivialIterator<T>::pointer TrivialIterator<T>::operator->()
{
    return data_.operator->();
}

template <typename T>
bool TrivialIterator<T>::operator==(const TrivialIterator<T>& rhs) const
{
    return *data_ == *rhs.data_;
}

template <typename T>
bool TrivialIterator<T>::operator!=(const TrivialIterator<T>& rhs) const
{
    return *data_ != *rhs.data_;
}

} // namespace vertex
