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

  [[nodiscard]] bool is_exhausted() const { return it == end; }

  [[nodiscard]] std::optional<OutputType<STL<C, T>>> next() {
    if (!is_exhausted()) { return std::reference_wrapper(*(it++)); }

    return {};
  }

 private:
  typename C<T>::const_iterator it;
  typename C<T>::const_iterator end;
};

template<template<typename...> typename C, typename T>
struct Types<STL<C, T>> {
  using Output = std::reference_wrapper<const T>;
};

/**
 * An iterator over an owned STL collection
 */
template<template<typename...> typename C, typename T>
struct STLMove : public Iterator<STLMove<C, T>> {
 public:
  explicit STLMove(C<T> &&_underlying)
      : underlying(std::move(_underlying)), it(underlying.begin()) {}

  [[nodiscard]] bool is_exhausted() const { return it == underlying.end(); }

  [[nodiscard]] std::optional<OutputType<STLMove<C, T>>> next() {
    if (!is_exhausted()) { return std::move(*(it++)); }

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

  [[nodiscard]] bool is_exhausted() const { return underlying.empty(); }

  [[nodiscard]] std::optional<OutputType<STLMove<std::set, T>>> next() {
    if (!is_exhausted()) {
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

  [[nodiscard]] bool is_exhausted() const { return it == end; }

  [[nodiscard]] std::optional<OutputType<STLPair<C, K, V>>> next() {
    if (!is_exhausted()) { return *(it++); }

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

  [[nodiscard]] bool is_exhausted() const { return it == underlying.end(); }

  [[nodiscard]] std::optional<OutputType<STLPairMove<C, K, V>>> next() {
    if (!is_exhausted()) { return std::move(*(it++)); }

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

  [[nodiscard]] bool is_exhausted() const { return i == N; }

  [[nodiscard]] std::optional<OutputType<Array<T, N>>> next() {
    if (!is_exhausted()) { return underlying[i++]; }

    return {};
  }

 private:
  size_t i;
  const std::array<T, N> &underlying;
};

template<typename T, size_t N>
struct Types<Array<T, N>> {
  using Output = std::reference_wrapper<const T>;
};

/**
 * An iterator over an owned array
 */
template<typename T, size_t N>
class ArrayMove : public Iterator<ArrayMove<T, N>> {
 public:
  explicit ArrayMove(std::array<T, N> &&underlying)
      : i(0), underlying(std::move(underlying)) {}

  [[nodiscard]] bool is_exhausted() const { return i == N; }

  [[nodiscard]] std::optional<OutputType<ArrayMove<T, N>>> next() {
    if (!is_exhausted()) { return std::move(underlying[i++]); }

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

template<typename F, typename I>
class Map : public Iterator<Map<F, I>> {
 public:
  explicit Map(F func, Iterator<I> &&underlying)
      : underlying(static_cast<I &&>(underlying)), func(func) {}
  Map(const Map &) = delete;
  Map(Map &&) noexcept = default;

  [[nodiscard]] bool is_exhausted() const { return underlying.is_exhausted(); }

  [[nodiscard]] std::optional<OutputType<Map<F, I>>> next() {
    auto content = underlying.next();

    if (content.has_value()) { return func(content.value()); }

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

  [[nodiscard]] bool is_exhausted() const { return underlying.is_exhausted(); }

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
      inner = func(std::move(outer_content.value()));
    }
  }

  FlatMap(const FlatMap &) = delete;
  FlatMap(FlatMap &&) noexcept = default;

  [[nodiscard]] bool is_exhausted() const { return outer.is_exhausted(); }

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

  void advance() {
    inner.advance();

    if (inner.at_end()) {
      outer.advance();
      inner = func(outer.content());
    }
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

  [[nodiscard]] bool is_exhausted() const { return underlying.is_exhausted(); }

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

  [[nodiscard]] bool is_exhausted() const { return underlying.is_exhausted(); }

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

  [[nodiscard]] bool is_exhausted() const { return underlying.is_exhausted(); }

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

  [[nodiscard]] bool is_exhausted() const { return underlying.is_exhausted(); }

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
  explicit Window(Iterator<I>&& iter) : m_underlying(static_cast<I&&>(iter)), m_start_index(0) {
    for (size_t i = 0; i < N; ++i) {
      m_elements[i] = m_underlying.next();
    }
  }

  [[nodiscard]] bool is_exhausted() const { return m_underlying.is_exhausted(); }

  [[nodiscard]] std::optional<OutputType<Window<N, I>>> next() {
    std::array<OutputType<I>, N> content;

    for (size_t i = 0; i < N; ++i) {
      size_t j = (m_start_index + i) % N;

      if (m_elements[j].has_value()) {
        content[i] = m_elements[j].value();

      } else {
        return {};

      }
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

template<typename T>
class Range : public Iterator<Range<T>> {
 public:
  explicit Range(T begin, T end, T step) : i(begin), end(end), step(step) {}

  [[nodiscard]] bool is_exhausted() const { return i >= end; }

  [[nodiscard]] std::optional<OutputType<Range>> next() {
    if (!is_exhausted()) {
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

  [[nodiscard]] bool is_exhausted() const { return false; }

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
  explicit Function(F func) : m_func(std::move(func)), m_next(m_func()) {}

  [[nodiscard]] bool is_exhausted() const {
    return !m_next.has_value();
  }

  [[nodiscard]] std::optional<OutputType<Function<F>>> next() {
    std::optional<OutputType<Function<F>>> out = std::move(m_next);
    m_next = m_func();

    return std::move(out);
  }

 private:
  F m_func;
  std::optional<OutputType<Function<F>>> m_next;
};

template<typename F>
struct Types<Function<F>> {
  using Output = typename std::result_of_t<F()>::value_type;
};

template<typename E, typename I>
class ChunkMap : public Iterator<ChunkMap<E, I>> {
 public:
  explicit ChunkMap(size_t size, E expr, Iterator<I> &&iter)
      : underlying(static_cast<I &&>(iter)), size(size), expr(expr) {}

  [[nodiscard]] bool is_exhausted() const { return underlying.is_exhausted(); }

  [[nodiscard]] std::optional<OutputType<ChunkMap<E, I>>> next() {
    if (!is_exhausted()) { return expr.apply(TakeRef<I>(size, underlying)); }

    return {};
  }

 private:
  I underlying;
  size_t size;
  E expr;
};

template<typename E, typename I>
struct Types<ChunkMap<E, I>> {
  using Output = expression::OutputType<E, I>;
};

template<typename I>
class Chunk : public Iterator<Chunk<I>> {
 public:
  explicit Chunk(size_t size, Iterator<I> &&iter)
      : underlying(static_cast<I &&>(iter)), size(size) {}

  [[nodiscard]] bool is_exhausted() const { return underlying.is_exhausted(); }

  [[nodiscard]] std::optional<OutputType<Chunk<I>>> next() {
    if (!is_exhausted()) { return TakeRef<I>(size, underlying); }

    return {};
  }

 private:
  I underlying;
  size_t size;
};

template<typename I>
struct Types<Chunk<I>> {
  using Output = TakeRef<I>;
};

template<typename I1, typename I2>
class Zip : public Iterator<Zip<I1, I2>> {
 public:
  explicit Zip(Iterator<I1> &&left, Iterator<I2> &&right)
      : left(static_cast<I1 &&>(left)), right(static_cast<I2 &&>(right)) {}

  [[nodiscard]] bool is_exhausted() const {
    return left.is_exhausted() || right.is_exhausted();
  }

  [[nodiscard]] std::optional<OutputType<Zip<I1, I2>>> next() {
    if (!is_exhausted()) {
      return std::make_pair(std::move(left.next().value()),
                            std::move(right.next().value()));
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

template<typename I1, typename I2>
class Concat : public Iterator<Concat<I1, I2>> {
 public:
  explicit Concat(Iterator<I1> &&left, Iterator<I2> &&right)
      : left(static_cast<I1 &&>(left)), right(static_cast<I2 &&>(right)) {}

  [[nodiscard]] bool is_exhausted() const { return right.is_exhausted(); }

  [[nodiscard]] std::optional<OutputType<Concat<I1, I2>>> next() {
    if (!left.is_exhausted()) { return left.next(); }

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

}// namespace colex::iterator
