#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

template<typename E1, typename E2>
class Composition : public Expression<Composition<E1, E2>> {
 public:
  explicit Composition(const E1 &e1, const E2 &e2) : e1(e1), e2(e2) {}
  explicit Composition(E1 &&e1, const E2 &e2) : e1(std::move(e1)), e2(e2) {}
  explicit Composition(const E1 &e1, E2 &&e2) : e1(e1), e2(std::move(e2)) {}
  explicit Composition(E1 &&e1, E2 &&e2)
          : e1(std::move(e1)), e2(std::move(e2)) {}

  template<typename I>
  OutputType<Composition<E1, E2>, I> apply(iterator::Iterator<I> &&iter) const {
    return e2.apply(e1.apply(std::move(iter)));
  }

 private:
  E1 e1;
  E2 e2;
};

template<typename E1, typename E2, typename I>
struct Types<Composition<E1, E2>, I> {
  using Output = OutputType<E2, OutputType<E1, I>>;
};

}
