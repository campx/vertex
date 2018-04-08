#include <memory>
#include <toolbox/Value.h>
#include <vertex/LinkIterator.h>

namespace objex
{
/******************
 * A_________     *
 *  \  \  \  \    *
 *   \  \  \  \   *
 *    \  \  \  \  *
 *     B  C  D  E *
 *****************/
/** Array provides a std::vector like interface atop a Hash tree of depth 1
 * Want to insert the value type of the Container, which may be a type which is
 * implicitly convertible to a std::pair */
template <typename Container>
class Array
{
private: // data
    using node_iterator = typename Container::iterator;
    toolbox::Value<Container> container_;
    node_iterator root_it_;

public: // typedefs
    using container_type = typename toolbox::Value<Container>::element_type;
    using value_type = typename container_type::value_type;
    using node_type = typename value_type::second_type;
    using link_type = typename node_type::link_type;
    using size_type = typename node_type::size_type;
    using difference_type = typename node_type::difference_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using iterator = vertex::LinkIterator<Container>;
    using const_iterator = const iterator;

public: // methods
    Array(Container container, node_iterator root_it);
    iterator begin() const;
    iterator end() const;
    bool empty() const;
    size_type length() const;
    void clear();
    iterator insert(const_iterator pos, const value_type& value);
    void push_back(const value_type& value);
};

template <typename Container>
Array<Container>::Array(Container container,
                        typename Array<Container>::node_iterator root_it)
    : container_(container), root_it_(std::move(root_it))
{
}

template <typename Container>
typename Array<Container>::iterator Array<Container>::begin() const
{
    return iterator(*container_, &root_it_->second, root_it_->second.cbegin());
}

template <typename Container>
typename Array<Container>::iterator Array<Container>::end() const
{
    return std::end(begin());
}

template <typename Container>
bool Array<Container>::empty() const
{
    return root_it_->second.empty();
}

template <typename Container>
typename Array<Container>::size_type Array<Container>::length() const
{
    return root_it_->second.length();
}

template <typename Container>
void Array<Container>::clear()
{
    root_it_->second.clear();
}

template <typename Container>
void Array<Container>::push_back(
    const typename Array<Container>::value_type& value)
{
    insert(end(), value);
}

template <typename Container>
typename Array<Container>::iterator
Array<Container>::insert(typename Array<Container>::const_iterator pos,
                         const typename Array<Container>::value_type& value)
{
    (*container_)[value.first] = value.second;
    auto result = root_it_->second.insert(pos.position(), value.first);
    return iterator(*container_, &root_it_->second, result.first);
}

} // namespace objex
