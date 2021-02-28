#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

template<typename T>
class Prepend : public Expression<Prepend<T>> {
 public:
  Prepend(std::vector<T> xs) : m_xs(std::move(xs)) {}
  Prepend(std::initializer_list<T> xs) : m_xs(xs) {}

  template<typename I>
  OutputType<Prepend<T>, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Concat(iterator::STL(m_xs), std::move(iter));
  }

 private:
  std::vector<T> m_xs;
};

template<typename T, typename I>
struct Types<Prepend<T>, I> {
  using Output = iterator::Concat<iterator::STL<std::vector, T>, I>;
};

}
