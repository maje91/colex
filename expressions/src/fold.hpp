#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

template<typename T, typename F>
class Fold : public Expression<Fold<T, F>> {
 public:
  explicit Fold(T initial, F func)
          : func(std::move(func)), initial(std::move(initial)) {}

  template<typename I>
  OutputType<Fold<T, F>, I> apply(iterator::Iterator<I> &&iter) const {
    T result = initial;

    for (auto content = iter.next(); content.has_value();
         content = iter.next()) {
      result = func(std::move(result), std::move(content.value()));
    }

    return result;
  }

 private:
  F func;
  T initial;
};

template<typename T, typename F, typename I>
struct Types<Fold<T, F>, I> {
  using Output = T;
};

template<typename F>
class Fold1 : public Expression<Fold1<F>> {
 public:
  explicit Fold1(F func) : func(func) {}

  template<typename I>
  OutputType<Fold1<F>, I> apply(iterator::Iterator<I> &&iter) const {
    OutputType<Fold1<F>, I> result = iter.next().value();

    for (auto content = iter.next(); content.has_value();
         content = iter.next()) {
      result = func(std::move(result), std::move(content.value()));
    }

    return result;
  }

 private:
  F func;
};

template<typename F, typename I>
struct Types<Fold1<F>, I> {
  using Output =
  std::result_of_t<F(iterator::OutputType<I>, iterator::OutputType<I>)>;
};

}
