#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

template<typename F>
class Map : public Expression<Map<F>> {
 public:
  explicit Map(F func) : func(func) {}

  template<typename I>
  OutputType<Map<F>, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Map<F, I>(func, std::move(iter));
  }

 private:
  F func;
};

template<typename F, typename I>
struct Types<Map<F>, I> {
  using Output = iterator::Map<F, I>;
};

}