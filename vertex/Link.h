#pragma once
#include <memory>

namespace vertex
{

/** A link contains a id with which an object may be looked up, plus some
 * additional metadata */
template <typename Identifier, typename T = void>
class Link
{
public:
    using id_type = Identifier;
    using value_type = T;

    explicit Link(id_type id = id_type(), value_type data = value_type());
    Link(const Link&) = default;
    Link(Link&&) = default;
    Link& operator=(const Link&) = default;
    Link& operator=(Link&&) = default;

    const id_type& id() const;
    const value_type& data() const;
    void id(const id_type& data);
    void data(const value_type& value);
    value_type& data();

    bool operator==(const Link& rhs) const;
    bool operator!=(const Link& rhs) const;
    bool operator<(const Link& rhs) const;
    bool operator>(const Link& rhs) const;

private:
    id_type id_;
    value_type data_;
};

template <typename Identifier, typename T>
Link<Identifier, T>::Link(Identifier id, T data)
    : id_(std::move(id)), data_(std::move(data))
{
}

template <typename Identifier, typename T>
const Identifier& Link<Identifier, T>::id() const
{
    return id_;
}

template <typename Identifier, typename T>
const T& Link<Identifier, T>::data() const
{
    return data_;
}

template <typename Identifier, typename T>
T& Link<Identifier, T>::data()
{
    return data_;
}

template <typename Identifier, typename T>
void Link<Identifier, T>::id(const Identifier& data)
{
    id_ = data;
}

template <typename Identifier, typename T>
void Link<Identifier, T>::data(const T& value)
{
    return data_ = value;
}

template <typename Identifier, typename T>
bool Link<Identifier, T>::operator==(const Link<Identifier, T>& rhs) const
{
    return id_ == rhs.id_ && data_ == rhs.data_;
}

template <typename Identifier, typename T>
bool Link<Identifier, T>::operator!=(const Link<Identifier, T>& rhs) const
{
    return !(*this == rhs);
}

template <typename Identifier, typename T>
bool Link<Identifier, T>::operator<(const Link<Identifier, T>& rhs) const
{
    auto result = false;
    if (id_ < rhs.id_)
    {
        result = true;
    }
    else if (id_ > rhs.id_)
    {
        result = false;
    }
    else if (data_ < rhs.data_)
    {
        result = true;
    }
    return result;
}

template <typename Identifier, typename T>
bool Link<Identifier, T>::operator>(const Link<Identifier, T>& rhs) const
{
    auto result = false;
    if (id_ > rhs.id_)
    {
        result = true;
    }
    else if (id_ < rhs.id_)
    {
        result = false;
    }
    else if (data_ > rhs.data_)
    {
        result = true;
    }
    return result;
}

} // namespace
