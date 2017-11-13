#pragma once
#include <utility>
#include <cstdint>
#include <map>

namespace vertex
{

template <typename VertexMap>
struct Predicate
{
    using result_type = bool;
    using key_type = typename VertexMap::key_type;
    using argument_type = std::pair<key_type, key_type>;
};

/** A predicate which always returns either true or false regardless of the
 * input */
template <typename VertexMap, bool Result>
struct NullaryPredicate : public Predicate<VertexMap>
{
    bool operator()(const argument_type& value) const;
};

template <typename VertexMap>
class MaxDepthPredicate : public Predicate<VertexMap>
{
public:
    MaxDepthPredicate(uint64_t max_depth);
    bool operator()(const argument_type& value);

private:
    uint64_t depth_;
    uint64_t max_depth_;
    std::map<key_type, uint64_t> sources_;
};

template <typename VertexMap, bool Result>
bool NullaryPredicate<VertexMap, Result>::
operator()(const typename NullaryPredicate<VertexMap, Result>::argument_type&
    value) const
{
    (void)value;
    return Result;
}

template <typename VertexMap>
MaxDepthPredicate<VertexMap>::MaxDepthPredicate(uint64_t max_depth)
    : depth_(0), max_depth_(max_depth)
{
}

template <typename VertexMap>
bool MaxDepthPredicate<VertexMap>::operator()(
    const typename MaxDepthPredicate<VertexMap>::argument_type& value)
{
    auto it = sources_.find(value.first);
    auto depth = uint64_t(it == sources_.end() ? 0 : it->second);
    sources_.insert(std::make_pair(value.first, depth));
    sources_.insert(std::make_pair(value.second, depth + 1));
    return depth < max_depth_;
}

}
