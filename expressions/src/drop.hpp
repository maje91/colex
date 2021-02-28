#pragma once

#include "../inc/interface.hpp"

namespace colex::expression {

class Drop : public Expression<Drop> {
 public:
  explicit Drop(size_t count) : count(count) {}

  template<typename I>
  OutputType<Drop, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Drop<I>(count, std::move(iter));
  }

 private:
  size_t count;
};

template<typename I>
struct Types<Drop, I> {
  using Output = iterator::Drop<I>;
};

}
