#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

template<typename T, typename F>
class Scan : public Expression<Scan<T, F>> {
 public:
  explicit Scan(T initial, F func) : m_func(func), m_initial(initial) {}

  template<typename I>
  OutputType<Scan<T, F>, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Scan<T, F, I>(m_initial, m_func, std::move(iter));
  }

 private:
  F m_func;
  T m_initial;
};

template<typename T, typename F, typename I>
struct Types<Scan<T, F>, I> {
  using Output = iterator::Scan<T, F, I>;
};

}
