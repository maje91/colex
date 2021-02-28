#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename I>
class Chunk : public Iterator<Chunk<I>> {
 public:
  explicit Chunk(size_t size, Iterator<I> &&iter)
          : underlying(static_cast<I &&>(iter)), size(size) {}

  Chunk(const Chunk &) = delete;
  Chunk(Chunk &&) noexcept = default;
  Chunk &operator=(Chunk &&) noexcept = default;
  Chunk &operator=(const Chunk &) = delete;

  [[nodiscard]] std::optional<OutputType<Chunk<I>>> next() {
    auto it = TakeRef<I>(size, underlying);
    auto first = it.next();
    if (first.has_value()) {
      return Concat(ArrayMove(std::array{std::move(first.value())}),
                    std::move(it));
    }

    return {};
  }

 private:
  I underlying;
  size_t size;
};

template<typename I>
struct Types<Chunk<I>> {
  using Output = Concat<ArrayMove<OutputType<I>, 1>, TakeRef<I>>;
};

}
