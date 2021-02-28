#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename I>
class Take : public Iterator<Take<I>> {
 public:
  explicit Take(size_t count, Iterator<I> &&iter)
          : i(0), take_count(count), underlying(static_cast<I &&>(iter)) {}

  Take(const Take &) = delete;
  Take(Take &&) noexcept = default;
  Take &operator=(Take &&) noexcept = default;
  Take &operator=(const Take &) = delete;

  [[nodiscard]] std::optional<OutputType<Take<I>>> next() {
    if (i++ < take_count) { return underlying.next(); }

    return {};
  }

 private:
  size_t i;
  size_t take_count;
  I underlying;
};

template<typename I>
struct Types<Take<I>> {
  using Output = OutputType<I>;
};

template<typename I>
class TakeRef : public Iterator<TakeRef<I>> {
 public:
  explicit TakeRef(size_t count, Iterator<I> &iter)
          : i(0), take_count(count), underlying(static_cast<I &>(iter)) {}

  TakeRef(const TakeRef &) = delete;
  TakeRef(TakeRef &&) noexcept = default;
  TakeRef &operator=(TakeRef &&) noexcept = default;
  TakeRef &operator=(const TakeRef &) = delete;

  [[nodiscard]] std::optional<OutputType<TakeRef<I>>> next() {
    if (i++ < take_count) { return underlying.next(); }

    return {};
  }

 private:
  size_t i;
  size_t take_count;
  I &underlying;
};

template<typename I>
struct Types<TakeRef<I>> {
  using Output = OutputType<I>;
};

}