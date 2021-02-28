#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename I1, typename I2,
        std::enable_if_t<std::is_same_v<OutputType<I1>, OutputType<I2>>, int> =
        0>
class Concat : public Iterator<Concat<I1, I2>> {
 public:
  explicit Concat(Iterator<I1> &&left, Iterator<I2> &&right)
          : left(static_cast<I1 &&>(left)), right(static_cast<I2 &&>(right)) {}

  Concat(const Concat &) = delete;
  Concat(Concat &&) noexcept = default;
  Concat &operator=(Concat &&) noexcept = default;
  Concat &operator=(const Concat &) = delete;

  [[nodiscard]] std::optional<OutputType<Concat<I1, I2>>> next() {
    auto left_content = left.next();

    if (left_content.has_value()) { return std::move(left_content.value()); }

    return right.next();
  }

 private:
  I1 left;
  I2 right;
};

template<typename I1, typename I2>
struct Types<Concat<I1, I2>> {
  using Output = OutputType<I1>;
};

}
