#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename T, typename F, typename I>
class Scan : public Iterator<Scan<T, F, I>> {
 public:
  explicit Scan(T initial, F func, Iterator<I> &&underlying)
          : m_underlying(static_cast<I&&>(underlying)), m_value(initial), m_func(func) {}

  Scan(const Scan &) = delete;
  Scan(Scan &&) noexcept = default;
  Scan &operator=(Scan &&) noexcept = default;
  Scan &operator=(const Scan &) = delete;

  [[nodiscard]] std::optional<OutputType<Scan<T, F, I>>> next() {
    if (m_value.has_value()) {
      T output = *m_value;
      m_value = {};

      auto x = m_underlying.next();

      if (x.has_value()) {
        m_value = m_func(output, x.value());
      }

      return output;
    }

    return {};
  }

 private:
  I m_underlying;
  std::optional<T> m_value;
  F m_func;
};

template<typename T, typename F, typename I>
struct Types<Scan<T, F, I>> {
  using Output = T;
};

}