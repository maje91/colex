#pragma once

#include "../inc/interface.hpp"

#include "iterators/inc/iterators.hpp"

#include <vector>

namespace colex::expression {

template<typename T>
class Append : public Expression<Append<T>> {
 public:
  Append(std::vector<T> xs) : m_xs(std::move(xs)) {}

  template<typename I>
  OutputType<Append<T>, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Concat(std::move(iter), iterator::STL(m_xs));
  }

 private:
  std::vector<T> m_xs;
};

template<typename T, typename I>
struct Types<Append<T>, I> {
  using Output = iterator::Concat<iterator::STL<std::vector, T>, I>;
};

}
