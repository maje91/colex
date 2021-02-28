#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

class Chunk : public Expression<Chunk> {
 public:
  explicit Chunk(size_t size) : size(size) {}

  template<typename I>
  OutputType<Chunk, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Chunk<I>(size, std::move(iter));
  }

 private:
  size_t size;
};

template<typename I>
struct Types<Chunk, I> {
  using Output = iterator::Chunk<I>;
};

}
