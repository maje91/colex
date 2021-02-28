#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

template<typename E>
class PartitionMap : public Expression<PartitionMap<E>> {
 public:
  explicit PartitionMap(std::vector<size_t> partition_sizes,
                        Expression<E> &&expr)
          : m_partition_sizes(std::move(partition_sizes)),
            m_expr(static_cast<E &&>(expr)) {}

  template<typename I>
  OutputType<PartitionMap<E>, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::PartitionMap<E, I>(m_partition_sizes, m_expr, std::move(iter));
  }

 private:
  std::vector<size_t> m_partition_sizes;
  E m_expr;
};

template<typename E, typename I>
struct Types<PartitionMap<E>, I> {
  using Output = iterator::PartitionMap<E, I>;
};

}
