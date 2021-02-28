#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

template<typename F>
class ForEach : public Expression<ForEach<F>> {
 public:
  explicit ForEach(F func) : func(func) {}

  template<typename I>
  OutputType<ForEach<F>, I> apply(iterator::Iterator<I> &&iter) const {
    for (auto content = iter.next(); content.has_value();
         content = iter.next()) {
      func(std::move(content.value()));
    }
  }

 private:
  F func;
};

template<typename F, typename I>
struct Types<ForEach<F>, I> {
  using Output = void;
};

}
