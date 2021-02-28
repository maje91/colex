#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename I1, typename I2>
class Zip : public Iterator<Zip<I1, I2>> {
 public:
  explicit Zip(Iterator<I1> &&left, Iterator<I2> &&right)
          : left(static_cast<I1 &&>(left)), right(static_cast<I2 &&>(right)) {}

  Zip(const Zip &) = delete;
  Zip(Zip &&) noexcept = default;
  Zip &operator=(Zip &&) noexcept = default;
  Zip &operator=(const Zip &) = delete;

  [[nodiscard]] std::optional<OutputType<Zip<I1, I2>>> next() {
    auto l = left.next();
    auto r = right.next();

    if (l.has_value() && r.has_value()) {
      return std::make_pair(std::move(l.value()), std::move(r.value()));
    }

    return {};
  }

 private:
  I1 left;
  I2 right;
};

template<typename I1, typename I2>
struct Types<Zip<I1, I2>> {
  using Output = std::pair<OutputType<I1>, OutputType<I2>>;
};

}
