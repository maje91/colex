#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

template<typename E>
class ChunkMap : public Expression<ChunkMap<E>> {
 public:
  explicit ChunkMap(size_t size, Expression<E> &&expr)
          : size(size), expr(static_cast<E &&>(expr)) {}

  template<typename I>
  OutputType<ChunkMap, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::ChunkMap<E, I>(size, expr, std::move(iter));
  }

 private:
  size_t size;
  E expr;
};

template<typename E, typename I>
struct Types<ChunkMap<E>, I> {
  using Output = iterator::ChunkMap<E, I>;
};

}
