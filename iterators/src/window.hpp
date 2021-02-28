#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<size_t N, typename I>
class Window : public Iterator<Window<N, I>> {
 public:
  explicit Window(Iterator<I> &&iter)
          : m_underlying(static_cast<I &&>(iter)), m_start_index(0) {
    for (size_t i = 0; i < N; ++i) { m_elements[i] = m_underlying.next(); }
  }

  Window(const Window &) = delete;
  Window(Window &&) noexcept = default;
  Window &operator=(Window &&) noexcept = default;
  Window &operator=(const Window &) = delete;

  [[nodiscard]] bool last_is_none() const {
    if (m_start_index == 0) { return !m_elements[N - 1].has_value(); }

    return !m_elements[m_start_index - 1].has_value();
  }

  [[nodiscard]] std::optional<OutputType<Window<N, I>>> next() {
    if (last_is_none()) { return {}; }

    std::array<OutputType<I>, N> content;
    for (size_t i = 0; i < N; ++i) {
      size_t j = (m_start_index + i) % N;
      content[i] = m_elements[j].value();
    }

    m_elements[m_start_index] = m_underlying.next();
    m_start_index = (m_start_index + 1) % N;

    return std::move(content);
  }

 private:
  I m_underlying;
  size_t m_start_index;
  std::array<std::optional<OutputType<I>>, N> m_elements;
};

template<size_t N, typename I>
struct Types<Window<N, I>> {
  using Output = std::array<OutputType<I>, N>;
};

}