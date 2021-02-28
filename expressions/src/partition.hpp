#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

class Partition : public Expression<Partition> {
 public:
  explicit Partition(std::vector<size_t> partition_sizes)
          : m_partition_sizes(std::move(partition_sizes)) {}

  template<typename I>
  OutputType<Partition, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Partition<I>(m_partition_sizes, std::move(iter));
  }

 private:
  std::vector<size_t> m_partition_sizes;
};

template<typename I>
struct Types<Partition, I> {
  using Output = iterator::Partition<I>;
};

}
