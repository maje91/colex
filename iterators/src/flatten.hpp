#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename I>
class Flatten : public Iterator<Flatten<I>> {
 public:
  explicit Flatten(Iterator<I> &&underlying)
          : m_outer(static_cast<I &&>(underlying)) {
    auto outer_content = m_outer.next();

    if (outer_content.has_value()) {
      m_inner.emplace(std::move(outer_content.value()));
    }
  }

  Flatten(const Flatten &) = delete;
  Flatten(Flatten &&) noexcept = default;
  Flatten &operator=(Flatten &&) noexcept = default;
  Flatten &operator=(const Flatten &) = delete;

  [[nodiscard]] std::optional<OutputType<Flatten<I>>> next() {
    if (m_inner.has_value()) {
      auto inner_content = m_inner.value().next();

      if (inner_content.has_value()) { return inner_content; }

      auto outer_content = m_outer.next();

      if (outer_content.has_value()) {
        m_inner.emplace(std::move(outer_content.value()));
        return next();
      }
    }

    return {};
  }

 private:
  I m_outer;
  std::optional<OutputType<I>> m_inner;
};

template<typename I>
struct Types<Flatten<I>> {
  using Output = OutputType<OutputType<I>>;
};

}