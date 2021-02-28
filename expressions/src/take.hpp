#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

class Take : public Expression<Take> {
 public:
  explicit Take(size_t count) : count(count) {}

  template<typename I>
  OutputType<Take, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Take<I>(count, std::move(iter));
  }

 private:
  size_t count;
};

template<typename I>
struct Types<Take, I> {
  using Output = iterator::Take<I>;
};

}
