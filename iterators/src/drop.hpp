#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename I>
class Drop : public Iterator<Drop<I>> {
 public:
  explicit Drop(size_t count, Iterator<I> &&iter)
          : underlying(static_cast<I &&>(iter)) {
    for (size_t i = 0; i < count; ++i) {
      if (!underlying.next().has_value()) { break; }
    }
  }

  Drop(const Drop &) = delete;
  Drop(Drop &&) noexcept = default;
  Drop &operator=(Drop &&) noexcept = default;
  Drop &operator=(const Drop &) = delete;

  [[nodiscard]] std::optional<OutputType<Drop<I>>> next() {
    return underlying.next();
  }

 private:
  I underlying;
};

template<typename I>
struct Types<Drop<I>> {
  using Output = OutputType<I>;
};

}