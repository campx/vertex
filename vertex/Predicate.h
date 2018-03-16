#pragma once
#include <cstdint>
#include <map>
#include <utility>

namespace vertex
{

/** A predicate which always returns either true or false regardless of the
 * input */
template <typename Container, bool Result>
struct NullaryPredicate
{
    using result_type = bool;
    using key_type = typename Container::key_type;
    using argument_type = std::pair<key_type, key_type>;
    bool operator()(const argument_type& value) const;
};

template <typename Container>
class MaxDepthPredicate
{
public:
    using result_type = bool;
    using key_type = typename Container::key_type;
    using argument_type = std::pair<key_type, key_type>;
    MaxDepthPredicate(uint64_t max_depth);
    bool operator()(const argument_type& value);

private:
    uint64_t depth_;
    uint64_t max_depth_;
    std::map<key_type, uint64_t> sources_;
};

template <typename Container, bool Result>
bool NullaryPredicate<Container, Result>::
operator()(const typename NullaryPredicate<Container, Result>::argument_type&
               value) const
{
    (void)value;
    return Result;
}

template <typename Container>
MaxDepthPredicate<Container>::MaxDepthPredicate(uint64_t max_depth)
    : depth_(0), max_depth_(max_depth)
{
}

template <typename Container>
bool MaxDepthPredicate<Container>::
operator()(const typename MaxDepthPredicate<Container>::argument_type& value)
{
    auto it = sources_.find(value.first);
    auto depth = uint64_t(it == sources_.end() ? 0 : it->second);
    sources_.insert(std::make_pair(value.first, depth));
    sources_.insert(std::make_pair(value.second, depth + 1));
    return depth < max_depth_;
}
}
