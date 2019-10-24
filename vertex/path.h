#pragma once

#include <vector>

namespace vertex {

template <typename Container>
using path = std::vector<typename Container::key_type>;

}  // namespace vertex
