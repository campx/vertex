#pragma once
#include <vertex/node.h>

namespace vertex {

template <typename Link, typename T = void>
class pod_node : public node<pod_node<Link, T>, Link, T> {
 public:
  using base_type = node<pod_node<Link, T>, Link, T>;
  using element_type = typename base_type::element_type;
  using container_type = typename base_type::container_type;
  using value_type = typename base_type::value_type;

  pod_node() = default;
  explicit pod_node(element_type data) : data_(std::move(data)) {}

  pod_node(element_type data, container_type links)
      : links_(std::move(links)), data_(std::move(data)) {}

  container_type& get_links() { return links_; }
  element_type& get_element() { return data_; };

 private:
  container_type links_;
  element_type data_;
};

}  // namespace vertex
