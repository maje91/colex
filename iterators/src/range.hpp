#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

template<typename T>
class Range : public Iterator<Range<T>> {
 public:
  explicit Range(T begin, T end, T step) : i(begin), end(end), step(step) {}

  Range(const Range &) = delete;
  Range(Range &&) noexcept = default;
  Range &operator=(Range &&) noexcept = default;
  Range &operator=(const Range &) = delete;

  [[nodiscard]] std::optional<OutputType<Range>> next() {
    if (i < end) {
      T value = i;
      i += step;

      return value;
    }

    return {};
  }

 private:
  T i;
  T end;
  T step;
};

template<typename T>
struct Types<Range<T>> {
  using Output = T;
};


template<typename T>
class OpenRange : public Iterator<OpenRange<T>> {
 public:
  explicit OpenRange(T begin, T step) : i(begin), step(step) {}

  OpenRange(const OpenRange &) = delete;
  OpenRange(OpenRange &&) noexcept = default;
  OpenRange &operator=(OpenRange &&) noexcept = default;
  OpenRange &operator=(const OpenRange &) = delete;

  [[nodiscard]] std::optional<OutputType<OpenRange>> next() {
    T value = i;
    i += step;

    return value;
  }

 private:
  T i;
  T step;
};

template<typename T>
struct Types<OpenRange<T>> {
  using Output = T;
};

}
