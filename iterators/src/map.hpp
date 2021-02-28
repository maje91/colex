#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename F, typename I>
class Map : public Iterator<Map<F, I>> {
 public:
  explicit Map(F func, Iterator<I> &&underlying)
          : underlying(static_cast<I &&>(underlying)), func(func) {}

  Map(const Map &) = delete;
  Map(Map &&) noexcept = default;
  Map &operator=(Map &&) noexcept = default;
  Map &operator=(const Map &) = delete;

  [[nodiscard]] std::optional<OutputType<Map<F, I>>> next() {
    auto content = underlying.next();

    if (content.has_value()) { return func(std::move(content.value())); }

    return {};
  }

 private:
  I underlying;
  F func;
};

template<typename F, typename I>
struct Types<Map<F, I>> {
  using Output = std::invoke_result_t<F, OutputType<I>>;
};

}

