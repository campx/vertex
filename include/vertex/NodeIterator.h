#pragma once
#include <vertex/Iterator.h>

namespace vertex
{

template <typename Traversal>
struct NodeAccessor
{
	using result_type = typename Traversal::vertex_type;
	using argument_type = Traversal;

	const result_type& operator() (const argument_type& t) const
	{
		return t.vertex();
	}
};

template<typename Traversal>
using NodeIterator = Iterator<Traversal, NodeAccessor<Traversal>>;

} // namespace vertex
