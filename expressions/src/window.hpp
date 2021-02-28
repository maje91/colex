#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

template<size_t N>
class Window : public Expression<Window<N>> {
 public:
  Window() = default;

  template<typename I>
  OutputType<Window<N>, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Window<N, I>(std::move(iter));
  }
};

template<size_t N, typename I>
struct Types<Window<N>, I> {
  using Output = iterator::Window<N, I>;
};

}
