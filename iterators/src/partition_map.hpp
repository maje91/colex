#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename E, typename I>
class PartitionMap : public Iterator<PartitionMap<E, I>> {
 public:
  explicit PartitionMap(std::vector<size_t> partition_sizes, E expr, Iterator<I> &&iter)
          : m_underlying(static_cast<I &&>(iter)),
            m_partition_sizes(std::move(partition_sizes)), m_partition_index(0),
            expr(std::move(expr)) {}

  PartitionMap(const PartitionMap &) = delete;
  PartitionMap(PartitionMap &&) noexcept = default;
  PartitionMap &operator=(PartitionMap &&) noexcept = default;
  PartitionMap &operator=(const PartitionMap &) = delete;

  [[nodiscard]] std::optional<OutputType<PartitionMap<E, I>>> next() {
    size_t n = m_partition_index == m_partition_sizes.size()
               ? std::numeric_limits<size_t>::max()
               : m_partition_sizes[m_partition_index++];

    auto it = TakeRef<I>(n, m_underlying);
    auto first = it.next();
    if (first.has_value()) {
      return expr.apply(Concat(ArrayMove(std::array{std::move(first.value())}),
                               std::move(it)));
    }

    return {};
  }

 private:
  I m_underlying;
  std::vector<size_t> m_partition_sizes;
  size_t m_partition_index;
  E expr;
};

template<typename E, typename I>
struct Types<PartitionMap<E, I>> {
  using Output = expression::OutputType<
  E, Concat<ArrayMove<OutputType<I>, 1>, TakeRef<I>>>;
};

}
