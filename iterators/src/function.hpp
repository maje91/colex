#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename F>
class Function : public Iterator<Function<F>> {
 public:
  explicit Function(F func) : m_func(std::move(func)) {}

  Function(const Function &) = delete;
  Function(Function &&) noexcept = default;
  Function &operator=(Function &&) noexcept = default;
  Function &operator=(const Function &) = delete;

  [[nodiscard]] std::optional<OutputType<Function<F>>> next() {
    return std::move(m_func());
  }

 private:
  F m_func;
};

template<typename F>
struct Types<Function<F>> {
  using Output = typename std::result_of_t<F()>::value_type;
};

}
