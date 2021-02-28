#pragma once

#include "../inc/interface.hpp"

#include <vector>
#include <limits>

namespace colex::iterator {

template<typename I>
class Partition : public Iterator<Partition<I>> {
 public:
  Partition(std::vector<size_t> partition_sizes, Iterator<I> &&underlying)
          : m_partition_index(0), m_partition_sizes(std::move(partition_sizes)),
            m_underlying(static_cast<I &&>(underlying)) {}

  Partition(const Partition &) = delete;
  Partition(Partition &&) noexcept = default;
  Partition &operator=(Partition &&) noexcept = default;
  Partition &operator=(const Partition &) = delete;

  [[nodiscard]] std::optional<OutputType<Partition<I>>> next() {
    size_t n = m_partition_index == m_partition_sizes.size()
               ? std::numeric_limits<size_t>::max()
               : m_partition_sizes[m_partition_index++];

    auto it = TakeRef<I>(n, m_underlying);
    auto first = it.next();
    if (first.has_value()) {
      return Concat(ArrayMove(std::array{std::move(first.value())}),
                    std::move(it));
    }

    return {};
  }

 private:
  size_t m_partition_index;
  std::vector<size_t> m_partition_sizes;
  I m_underlying;
};

template<typename I>
struct Types<Partition<I>> {
  using Output = Concat<ArrayMove<OutputType<I>, 1>, TakeRef<I>>;
};

}
