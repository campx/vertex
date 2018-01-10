#pragma once
#include "TestNode.h"
#include <map>
#include <objex/Forest.h>

namespace objex
{

using NodeMap = std::map<TestNode::value_type, TestNode>;
using EdgeMap = std::multimap<TestNode::value_type, TestNode::value_type>;

using TestForest = Forest<NodeMap, EdgeMap>;

} // namespace objex
