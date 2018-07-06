#include <memory>
#include <toolbox/Value.h>
#include <vertex/LinkIterator.h>

namespace vertex
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
    Container* container_;
    node_iterator root_it_;

public: // typedefs
    using container_type = Container;
    using value_type = typename container_type::value_type;
    using node_type = typename value_type::second_type;
    using link_type = typename node_type::link_type;
    using size_type = typename node_type::container_type::size_type;
    using difference_type =
    typename node_type::container_type::difference_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using iterator = vertex::LinkIterator<Container>;
    using const_iterator = const iterator;

public: // methods
    explicit Array(Container& container);

    Array(Container& container, node_iterator root_it);

    iterator begin() const;

    iterator end() const;

    bool empty() const;

    size_type length() const;

    void clear();

    node_iterator root() const;

    void root(node_iterator it);

    iterator insert(const_iterator pos, const value_type& value);

    void push_back(const value_type& value);
};

template <typename Container>
Array<Container>::Array(Container& container)
    : container_(&container), root_it_(container_->end())
{

}

template <typename Container>
Array<Container>::Array(Container& container,
                        typename Array<Container>::node_iterator root_it)
    : container_(&container), root_it_(std::move(root_it))
{
}

template <typename Container>
typename Array<Container>::iterator Array<Container>::begin() const
{
    return iterator(*container_, &root_it_->second,
                    root_it_->second.links().cbegin());
}

template <typename Container>
typename Array<Container>::iterator Array<Container>::end() const
{
    return std::end(begin());
}

template <typename Container>
bool Array<Container>::empty() const
{
    return root_it_->second.links().empty();
}

template <typename Container>
typename Array<Container>::size_type Array<Container>::length() const
{
    return root_it_->second.links().size();
}

template <typename Container>
void Array<Container>::clear()
{
    root_it_->second.links().clear();
}

template <typename Container>
typename Array<Container>::node_iterator Array<Container>::root() const
{
    return root_it_;
}

template <typename Container>
void Array<Container>::root(node_iterator it)
{
    root_it_ = std::move(it);
}

template <typename Container>
void
Array<Container>::push_back(const typename Array<Container>::value_type& value)
{
    insert(end(), value);
}

template <typename Container>
typename Array<Container>::iterator
Array<Container>::insert(typename Array<Container>::const_iterator pos,
                         const typename Array<Container>::value_type& value)
{
    (*container_)[value.first] = value.second;
    auto result = root_it_->second.links().insert(pos.position(), value.first);
    return iterator(*container_, &root_it_->second, result);
}

} // namespace vertex
