#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

class Flatten : public Expression<Flatten> {
 public:
  template<typename I>
  OutputType<Flatten, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Flatten<I>(std::move(iter));
  }
};

template<typename I>
struct Types<Flatten, I> {
  using Output = iterator::Flatten<I>;
};

}
