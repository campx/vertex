#pragma once
#include "TestNode.h"
#include <map>
#include <merkle/Forest.h>

namespace merkle
{

using NodeMap = std::map<TestNode::value_type, TestNode>;
using LinkMap = std::multimap<TestNode::value_type, TestNode::value_type>;

using TestForest = Forest<NodeMap, LinkMap>;

} // namespace merkle
