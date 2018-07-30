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
/** Array provides a std::vector like interface atop a tree of depth 1 */
template <typename Container>
class Array
{
private: // data
    using node_iterator = typename Container::iterator;
    Container* container_;
    node_iterator root_it_;

public:
    // typedefs
    using container_type = Container;
    using value_type = typename container_type::value_type;
    using node_type = typename value_type::second_type;
    using link_type = typename node_type::link_type;
    using size_type = typename node_type::container_type::size_type;
    using allocator_type = typename node_type::container_type::allocator_type;
    using key_type = typename node_type::link_type;
    using difference_type =
    typename node_type::container_type::difference_type;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    using iterator = vertex::LinkIterator<Container>;
    using const_iterator = vertex::LinkIterator<const Container>;

    // methods
    Array();
    explicit Array(Container& container);
    Array(Container& container, node_iterator root_it);

    iterator begin() const;
    const_iterator cbegin() const;
    iterator end() const;
    const_iterator cend() const;

    bool empty() const;
    size_type length() const;
    void clear();
    void root(node_iterator it);
    iterator insert(const_iterator pos, const value_type& value);
    void push_back(const value_type& value);

    node_iterator root() const;
};

template <typename Container>
Array<Container>::Array()
{
}

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
                    root_it_->second.links().begin());
}

template <typename Container>
typename Array<Container>::const_iterator Array<Container>::cbegin() const
{
    return const_iterator(*container_, &root_it_->second,
                          root_it_->second.links().cbegin());
}

template <typename Container>
typename Array<Container>::iterator Array<Container>::end() const
{
    return std::end(begin());
}

template <typename Container>
typename Array<Container>::const_iterator Array<Container>::cend() const
{
    return std::cend(cbegin());
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
    insert(cend(), value);
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
