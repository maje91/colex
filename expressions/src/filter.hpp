#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

template<typename F>
class Filter : public Expression<Filter<F>> {
 public:
  explicit Filter(F predicate) : predicate(predicate) {}

  template<typename I>
  OutputType<Filter<F>, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Filter<F, I>(predicate, std::move(iter));
  }

 private:
  F predicate;
};

template<typename F, typename I>
struct Types<Filter<F>, I> {
  using Output = iterator::Filter<F, I>;
};

}
