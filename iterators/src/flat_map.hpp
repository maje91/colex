#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename F, typename I>
class FlatMap : public Iterator<FlatMap<F, I>> {
 public:
  explicit FlatMap(F func, Iterator<I> &&underlying)
          : m_outer(static_cast<I &&>(underlying)), m_func(std::move(func)) {
    auto outer_content = m_outer.next();

    if (outer_content.has_value()) {
      m_inner.emplace(m_func(std::move(outer_content.value())));
    }
  }

  FlatMap(const FlatMap &) = delete;
  FlatMap(FlatMap &&) noexcept = default;
  FlatMap &operator=(FlatMap &&) noexcept = default;
  FlatMap &operator=(const FlatMap &) = delete;

  [[nodiscard]] std::optional<OutputType<FlatMap<F, I>>> next() {
    if (m_inner.has_value()) {
      auto inner_content = m_inner.value().next();

      if (inner_content.has_value()) { return inner_content; }

      auto outer_content = m_outer.next();

      if (outer_content.has_value()) {
        m_inner.emplace(m_func(std::move(outer_content.value())));
        return next();
      }
    }

    return {};
  }

 private:
  I m_outer;
  std::optional<std::invoke_result_t<F, OutputType<I>>> m_inner;
  F m_func;
};

template<typename F, typename I>
struct Types<FlatMap<F, I>> {
  using Output = OutputType<std::invoke_result_t<F, OutputType<I>>>;
};

}
