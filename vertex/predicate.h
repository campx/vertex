#pragma once
#include <vertex/edge.h>
#include <cstdint>
#include <map>
#include <utility>

namespace vertex {

template <typename Container>
class MaxDepthPredicate {
 public:
  explicit MaxDepthPredicate(uint64_t max_depth);
  bool operator()(const edge<Container>& value);

 private:
  uint64_t depth_;
  uint64_t max_depth_;
  std::map<typename Container::key_type, uint64_t> sources_;
};

template <typename Container>
MaxDepthPredicate<Container>::MaxDepthPredicate(uint64_t max_depth)
    : depth_(0), max_depth_(max_depth) {}

template <typename Container>
bool MaxDepthPredicate<Container>::operator()(const edge<Container>& value) {
  auto it = sources_.find(value.source());
  auto depth = uint64_t(it == sources_.end() ? 0 : it->second);
  sources_.insert(std::make_pair(value.source(), depth));
  sources_.insert(std::make_pair(value.target(), depth + 1));
  return depth < max_depth_;
}
}  // namespace vertex
