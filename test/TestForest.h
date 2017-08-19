#pragma once
#include "TestVertex.h"
#include <map>
#include <vertex/Forest.h>

namespace vertex
{

using VertexMap = std::map<TestVertex::value_type, TestVertex>;
using EdgeMap = std::multimap<TestVertex::value_type, TestVertex::value_type>;

using TestForest = Forest<VertexMap, EdgeMap>;

} // namespace vertex
