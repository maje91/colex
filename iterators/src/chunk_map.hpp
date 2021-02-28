#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename E, typename I>
class ChunkMap : public Iterator<ChunkMap<E, I>> {
 public:
  explicit ChunkMap(size_t size, E expr, Iterator<I> &&iter)
          : underlying(static_cast<I &&>(iter)), size(size), expr(std::move(expr)) {}

  ChunkMap(const ChunkMap &) = delete;
  ChunkMap(ChunkMap &&) noexcept = default;
  ChunkMap &operator=(ChunkMap &&) noexcept = default;
  ChunkMap &operator=(const ChunkMap &) = delete;

  [[nodiscard]] std::optional<OutputType<ChunkMap<E, I>>> next() {
    auto it = TakeRef<I>(size, underlying);
    auto first = it.next();
    if (first.has_value()) {
      return expr.apply(Concat(ArrayMove(std::array{std::move(first.value())}),
                               std::move(it)));
    }

    return {};
  }

 private:
  I underlying;
  size_t size;
  E expr;
};

template<typename E, typename I>
struct Types<ChunkMap<E, I>> {
  using Output = expression::OutputType<
  E, Concat<ArrayMove<OutputType<I>, 1>, TakeRef<I>>>;
};

}
