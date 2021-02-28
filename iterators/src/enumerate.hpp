#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename I>
class Enumerate : public Iterator<Enumerate<I>> {
 public:
  explicit Enumerate(Iterator<I> &&iter)
          : underlying(static_cast<I &&>(iter)), i(0) {}

  Enumerate(const Enumerate &) = delete;
  Enumerate(Enumerate &&) noexcept = default;
  Enumerate &operator=(Enumerate &&) noexcept = default;
  Enumerate &operator=(const Enumerate &) = delete;

  [[nodiscard]] std::optional<OutputType<Enumerate<I>>> next() {
    auto content = underlying.next();

    if (content.has_value()) {
      return std::make_pair(i++, std::move(content.value()));
    }

    return {};
  }

 private:
  I underlying;
  size_t i;
};

template<typename I>
struct Types<Enumerate<I>> {
  using Output = std::pair<size_t, OutputType<I>>;
};

}