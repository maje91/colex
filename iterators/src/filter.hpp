#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename F, typename I>
class Filter : public Iterator<Filter<F, I>> {
 public:
  explicit Filter(F predicate, Iterator<I> &&underlying)
          : underlying(static_cast<I &&>(underlying)), predicate(predicate) {}

  Filter(const Filter &) = delete;
  Filter(Filter &&) noexcept = default;
  Filter &operator=(Filter &&) noexcept = default;
  Filter &operator=(const Filter &) = delete;

  [[nodiscard]] std::optional<OutputType<Filter<F, I>>> next() {
    for (auto content = underlying.next(); content.has_value();
         content = underlying.next()) {
      if (predicate(content.value())) { return std::move(content.value()); }
    }

    return {};
  }

 private:
  I underlying;
  F predicate;
};

template<typename F, typename I>
struct Types<Filter<F, I>> {
  using Output = OutputType<I>;
};

}
