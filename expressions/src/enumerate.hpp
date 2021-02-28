#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

class Enumerate : public Expression<Enumerate> {
 public:
  explicit Enumerate() {}

  template<typename I>
  OutputType<Enumerate, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Enumerate<I>(std::move(iter));
  }
};

template<typename I>
struct Types<Enumerate, I> {
  using Output = iterator::Enumerate<I>;
};

}
