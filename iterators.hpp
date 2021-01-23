#pragma once

#include "expression_interface.hpp"
#include "iterator_interface.hpp"

#include <array>
#include <functional>
#include <set>
#include <utility>

namespace colex::iterator {

/**
 * An iterator over a borrowed STL collection
 */
template<template<typename...> typename C, typename T>
class STL : public Iterator<STL<C, T>> {
 public:
  explicit STL(const C<T> &underlying)
      : it(underlying.begin()), end(underlying.end()) {}

  [[nodiscard]] std::optional<OutputType<STL<C, T>>> next() {
    if (it != end) { return *(it++); }

    return {};
  }

 private:
  typename C<T>::const_iterator it;
  typename C<T>::const_iterator end;
};

template<template<typename...> typename C, typename T>
struct Types<STL<C, T>> {
  using Output = T;
};

/**
 * An iterator over an owned STL collection
 */
template<template<typename...> typename C, typename T>
struct STLMove : public Iterator<STLMove<C, T>> {
 public:
  explicit STLMove(C<T> &&_underlying)
      : underlying(std::move(_underlying)), it(underlying.begin()) {}

  [[nodiscard]] std::optional<OutputType<STLMove<C, T>>> next() {
    if (it != underlying.end()) { return std::move(*(it++)); }

    return {};
  }

 private:
  C<T> underlying;
  typename C<T>::iterator it;
};

/**
 * `std::set` requires special treatment
 */
template<typename T>
struct STLMove<std::set, T> : public Iterator<STLMove<std::set, T>> {
 public:
  explicit STLMove(std::set<T> &&underlying)
      : underlying(std::move(underlying)) {}

  [[nodiscard]] std::optional<OutputType<STLMove<std::set, T>>> next() {
    if (!underlying.empty()) {
      return std::move(underlying.extract(underlying.begin()).value());
    }

    return {};
  }

 private:
  std::set<T> underlying;
};

template<template<typename...> typename C, typename T>
struct Types<STLMove<C, T>> {
  using Output = T;
};

/**
 * An iterator over an STL collection where the elements are pairs.
 */
template<template<typename...> typename C, typename K, typename V>
class STLPair : public Iterator<STLPair<C, K, V>> {
 public:
  explicit STLPair(const C<K, V> &underlying)
      : it(underlying.begin()), end(underlying.end()) {}

  [[nodiscard]] std::optional<OutputType<STLPair<C, K, V>>> next() {
    if (it != end) { return *(it++); }

    return {};
  }

 private:
  typename C<K, V>::const_iterator it;
  typename C<K, V>::const_iterator end;
};

template<template<typename...> typename C, typename K, typename V>
struct Types<STLPair<C, K, V>> {
  using Output = std::pair<K, V>;
};

/**
 * An iterator over an owned STL collection where the elements are pairs.
 */
template<template<typename...> typename C, typename K, typename V>
class STLPairMove : public Iterator<STLPairMove<C, K, V>> {
 public:
  explicit STLPairMove(C<K, V> &&_underlying)
      : underlying(std::move(_underlying)), it(underlying.begin()) {}

  [[nodiscard]] std::optional<OutputType<STLPairMove<C, K, V>>> next() {
    if (it != underlying.end()) { return std::move(*(it++)); }

    return {};
  }

 private:
  C<K, V> underlying;
  typename C<K, V>::iterator it;
};

template<template<typename...> typename C, typename K, typename V>
struct Types<STLPairMove<C, K, V>> {
  using Output = std::pair<K, V>;
};

/**
 * An iterator over a borrowed array
 */
template<typename T, size_t N>
class Array : public Iterator<Array<T, N>> {
 public:
  explicit Array(const std::array<T, N> &underlying)
      : i(0), underlying(underlying) {}

  [[nodiscard]] std::optional<OutputType<Array<T, N>>> next() {
    if (i < N) { return underlying[i++]; }

    return {};
  }

 private:
  size_t i;
  const std::array<T, N> &underlying;
};

template<typename T, size_t N>
struct Types<Array<T, N>> {
  using Output = T;
};

/**
 * An iterator over an owned array
 */
template<typename T, size_t N>
class ArrayMove : public Iterator<ArrayMove<T, N>> {
 public:
  explicit ArrayMove(std::array<T, N> &&underlying)
      : i(0), underlying(std::move(underlying)) {}

  [[nodiscard]] std::optional<OutputType<ArrayMove<T, N>>> next() {
    if (i < N) { return std::move(underlying[i++]); }

    return {};
  }

 private:
  size_t i;
  std::array<T, N> underlying;
};

template<typename T, size_t N>
struct Types<ArrayMove<T, N>> {
  using Output = T;
};

template<typename T>
class Pointer : public Iterator<Pointer<T>> {
 public:
  explicit Pointer(const T *underlying, size_t element_count)
      : m_ptr(underlying), m_end(m_ptr + element_count) {}

  [[nodiscard]] std::optional<OutputType<Pointer<T>>> next() {
    if (m_ptr != m_end) { return *(m_ptr++); }

    return {};
  }

 private:
  const T *m_ptr;
  const T *m_end;
};

template<typename T>
struct Types<Pointer<T>> {
  using Output = T;
};

template<typename T>
class PointerMove : public Iterator<PointerMove<T>> {
 public:
  explicit PointerMove(T *underlying, size_t element_count)
      : m_ptr(underlying), m_end(m_ptr + element_count) {}

  [[nodiscard]] std::optional<OutputType<PointerMove<T>>> next() {
    if (m_ptr != m_end) { return std::move(*(m_ptr++)); }

    return {};
  }

 private:
  T *m_ptr;
  T *m_end;
};

template<typename T>
struct Types<PointerMove<T>> {
  using Output = T;
};

template<typename F, typename I>
class Map : public Iterator<Map<F, I>> {
 public:
  explicit Map(F func, Iterator<I> &&underlying)
      : underlying(static_cast<I &&>(underlying)), func(func) {}
  Map(const Map &) = delete;
  Map(Map &&) noexcept = default;

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

template<typename F, typename I>
class Filter : public Iterator<Filter<F, I>> {
 public:
  explicit Filter(F predicate, Iterator<I> &&underlying)
      : underlying(static_cast<I &&>(underlying)), predicate(predicate) {}
  Filter(const Filter &) = delete;
  Filter(Filter &&) noexcept = default;

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

template<typename F, typename I>
class FlatMap : public Iterator<FlatMap<F, I>> {
 public:
  explicit FlatMap(F func, Iterator<I> &&underlying)
      : outer(static_cast<I &&>(underlying)), func(func) {
    auto outer_content = outer.next();

    if (outer_content.has_value()) {
      inner = std::move(func(std::move(outer_content.value())));
    }
  }

  FlatMap(const FlatMap &) = delete;
  FlatMap(FlatMap &&) noexcept = default;
  FlatMap& operator=(FlatMap &&) noexcept = default;

  [[nodiscard]] std::optional<OutputType<FlatMap<F, I>>> next() {
    if (inner.has_value()) {
      auto inner_content = inner.value().next();

      if (inner_content.has_value()) { return inner_content; }

      auto outer_content = outer.next();

      if (outer_content.has_value()) {
        inner = func(std::move(outer_content.value()));
        return next();
      }
    }

    return {};
  }

 private:
  I outer;
  std::optional<std::invoke_result_t<F, OutputType<I>>> inner;
  F func;
};

template<typename F, typename I>
struct Types<FlatMap<F, I>> {
  using Output = OutputType<std::invoke_result_t<F, OutputType<I>>>;
};

template<typename I>
class Take : public Iterator<Take<I>> {
 public:
  explicit Take(size_t count, Iterator<I> &&iter)
      : i(0), take_count(count), underlying(static_cast<I &&>(iter)) {}

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

template<typename I>
class Drop : public Iterator<Drop<I>> {
 public:
  explicit Drop(size_t count, Iterator<I> &&iter)
      : underlying(static_cast<I &&>(iter)) {
    for (size_t i = 0; i < count; ++i) {
      if (!underlying.next().has_value()) { break; }
    }
  }

  [[nodiscard]] std::optional<OutputType<Drop<I>>> next() {
    return underlying.next();
  }

 private:
  I underlying;
};

template<typename I>
struct Types<Drop<I>> {
  using Output = OutputType<I>;
};

template<typename I>
class Enumerate : public Iterator<Enumerate<I>> {
 public:
  explicit Enumerate(Iterator<I> &&iter)
      : underlying(static_cast<I &&>(iter)), i(0) {}

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

template<size_t N, typename I>
class Window : public Iterator<Window<N, I>> {
 public:
  explicit Window(Iterator<I> &&iter)
      : m_underlying(static_cast<I &&>(iter)), m_start_index(0) {
    for (size_t i = 0; i < N; ++i) { m_elements[i] = m_underlying.next(); }
  }

  [[nodiscard]] bool last_is_none() const {
    if (m_start_index == 0) { return !m_elements[N - 1].has_value(); }

    return !m_elements[m_start_index - 1].has_value();
  }

  template<typename T,
           std::enable_if_t<std::is_same_v<T, std::array<OutputType<I>, 2>>,
                            int> = 0>
  T make_content() {
    return {
            m_elements[m_start_index].value(),
            m_elements[(m_start_index + 1) % N].value(),
    };
  }

  template<typename T,
           std::enable_if_t<std::is_same_v<T, std::array<OutputType<I>, 3>>,
                            int> = 0>
  T make_content() {
    return {
            m_elements[m_start_index].value(),
            m_elements[(m_start_index + 1) % N].value(),
            m_elements[(m_start_index + 2) % N].value(),
    };
  }

  template<typename T,
           std::enable_if_t<std::is_same_v<T, std::array<OutputType<I>, 4>>,
                            int> = 0>
  T make_content() {
    return {
            m_elements[m_start_index].value(),
            m_elements[(m_start_index + 1) % N].value(),
            m_elements[(m_start_index + 2) % N].value(),
            m_elements[(m_start_index + 3) % N].value(),
    };
  }

  template<typename T,
           std::enable_if_t<std::is_same_v<T, std::array<OutputType<I>, 5>>,
                            int> = 0>
  T make_content() {
    return {
            m_elements[m_start_index].value(),
            m_elements[(m_start_index + 1) % N].value(),
            m_elements[(m_start_index + 2) % N].value(),
            m_elements[(m_start_index + 3) % N].value(),
            m_elements[(m_start_index + 4) % N].value(),
    };
  }

  [[nodiscard]] std::optional<OutputType<Window<N, I>>> next() {
    if (last_is_none()) { return {}; }

    std::array<OutputType<I>, N> content =
            make_content<std::array<OutputType<I>, N>>();

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

template<typename T>
class Range : public Iterator<Range<T>> {
 public:
  explicit Range(T begin, T end, T step) : i(begin), end(end), step(step) {}

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

template<typename F>
class Function : public Iterator<Function<F>> {
 public:
  explicit Function(F func) : m_func(std::move(func)) {}

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

template<typename I1, typename I2,
        std::enable_if_t<std::is_same_v<OutputType<I1>, OutputType<I2>>, int> = 0>
class Concat : public Iterator<Concat<I1, I2>> {
 public:
  explicit Concat(Iterator<I1> &&left, Iterator<I2> &&right)
      : left(static_cast<I1 &&>(left)), right(static_cast<I2 &&>(right)) {}

  [[nodiscard]] std::optional<OutputType<Concat<I1, I2>>> next() {
    auto left_content = left.next();

    if (left_content.has_value()) { return std::move(left_content.value()); }

    return right.next();
  }

 private:
  I1 left;
  I2 right;
};

template<typename I1, typename I2>
struct Types<Concat<I1, I2>> {
  using Output = OutputType<I1>;
};

template<typename E, typename I>
class ChunkMap : public Iterator<ChunkMap<E, I>> {
 public:
  explicit ChunkMap(size_t size, E expr, Iterator<I> &&iter)
      : underlying(static_cast<I &&>(iter)), size(size), expr(expr) {}

  [[nodiscard]] std::optional<OutputType<ChunkMap<E, I>>> next() {
    auto it = TakeRef<I>(size, underlying);
    auto first = it.next();
    if (first.has_value()) {
      return expr.apply(
              Concat(ArrayMove(std::array{std::move(first.value())}), std::move(it)));
    }

    return {};
  }

 private:
  I underlying;
  size_t size;
  E expr;
};

template<typename E, typename I>
struct Types<ChunkMap<E, I>> {
  using Output = expression::OutputType<E, Concat<ArrayMove<OutputType<I>, 1>, TakeRef<I>>>;
};

template<typename I>
class Chunk : public Iterator<Chunk<I>> {
 public:
  explicit Chunk(size_t size, Iterator<I> &&iter)
      : underlying(static_cast<I &&>(iter)), size(size) {}

  [[nodiscard]] std::optional<OutputType<Chunk<I>>> next() {
    auto it = TakeRef<I>(size, underlying);
    auto first = it.next();
    if (first.has_value()) {
      return Concat(ArrayMove(std::array{std::move(first.value())}), std::move(it));
    }

    return {};
  }

 private:
  I underlying;
  size_t size;
};

template<typename I>
struct Types<Chunk<I>> {
  using Output = Concat<ArrayMove<OutputType<I>, 1>, TakeRef<I>>;
};

template<typename I1, typename I2>
class Zip : public Iterator<Zip<I1, I2>> {
 public:
  explicit Zip(Iterator<I1> &&left, Iterator<I2> &&right)
      : left(static_cast<I1 &&>(left)), right(static_cast<I2 &&>(right)) {}

  [[nodiscard]] std::optional<OutputType<Zip<I1, I2>>> next() {
    auto l = left.next();
    auto r = right.next();

    if (l.has_value() && r.has_value()) {
      return std::make_pair(std::move(l.value()), std::move(r.value()));
    }

    return {};
  }

 private:
  I1 left;
  I2 right;
};

template<typename I1, typename I2>
struct Types<Zip<I1, I2>> {
  using Output = std::pair<OutputType<I1>, OutputType<I2>>;
};

}// namespace colex::iterator
