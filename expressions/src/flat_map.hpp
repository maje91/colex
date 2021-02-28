#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

template<typename F>
class FlatMap : public Expression<FlatMap<F>> {
 public:
  explicit FlatMap(F func) : func(func) {}

  template<typename I>
  OutputType<FlatMap<F>, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::FlatMap<F, I>(func, std::move(iter));
  }

 private:
  F func;
};

template<typename F, typename I>
struct Types<FlatMap<F>, I> {
  using Output = iterator::FlatMap<F, I>;
};

}
