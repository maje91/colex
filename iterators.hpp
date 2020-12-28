#pragma once

#include <set>
#include <utility>

namespace colex::iterator {

/**
 * Base for types associated with an iterator.
 * Must be specialized for each iterator.
 * The aliases just below specifies which
 * types that must be defined.
 */
template<typename I>
struct Types;

/**
 * The type of elements that is output by an iterator
 */
template<typename I>
using OutputType = typename Types<I>::Output;

/**
 * Base for all iterators
 */
template<typename I>
struct Iterator {
  /**
   * Returns the next item. None if the iterator is exhausted.
   */
  [[nodiscard]] std::optional<OutputType<I>> next() {
    return static_cast<I &>(*this).next();
  }
};

/**
 * An iterator over a borrowed STL collection
 */
template<template<typename...> typename C, typename T>
class STL : public Iterator<STL<C, T>> {
 public:
  explicit STL(const C<T> &underlying) : it(underlying.begin()), end(underlying.end()) {}

  [[nodiscard]] std::optional<OutputType<STL<C, T>>> next() {
    if (it != end) {
      return std::reference_wrapper(*(it++));
    }

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
  explicit STLMove(C<T> &&_underlying) : underlying(std::move(_underlying)), it(underlying.begin()) {}

  [[nodiscard]] std::optional<OutputType<STLMove<C, T>>> next() {
    if (it != underlying.end()) {
      return std::move(*(it++));
    }

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
  explicit STLMove(std::set<T> &&underlying) : underlying(std::move(underlying)) {}

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
  explicit STLPair(const C<K, V> &underlying) : it(underlying.begin()), end(underlying.end()) {}

  [[nodiscard]] std::optional<OutputType<STLPair<C, K, V>>> next() {
    if (it != end) {
      return *(it++);
    }

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
  explicit STLPairMove(C<K, V> &&_underlying) : underlying(std::move(_underlying)), it(underlying.begin()) {}

  [[nodiscard]] std::optional<OutputType<STLPairMove<C, K, V>>> next() {
    if (it != underlying.end()) {
      return std::move(*(it++));
    }

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
  explicit Array(const std::array<T, N> &underlying) : i(0), underlying(underlying) {}

  [[nodiscard]] std::optional<OutputType<Array<T, N>>> next() {
    if (i < N) {
      return underlying[i++];
    }

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
  explicit ArrayMove(std::array<T, N> &&underlying) : i(0), underlying(std::move(underlying)) {}

  [[nodiscard]] std::optional<OutputType<ArrayMove<T, N>>> next() {
    if (i < N) {
      return std::move(underlying[i++]);
    }

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
  explicit Map(F func, Iterator<I> &&underlying) : underlying(static_cast<I &&>(underlying)), func(func) {}
  Map(const Map &) = delete;
  Map(Map &&) noexcept = default;

  [[nodiscard]] std::optional<OutputType<Map<F, I>>> next() {
    auto content = underlying.next();

    if (content.has_value()) {
      return func(content.value());
    }

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
  explicit Filter(F predicate, Iterator<I> &&underlying) : underlying(static_cast<I &&>(underlying)), predicate(predicate) {}
  Filter(const Filter &) = delete;
  Filter(Filter &&) noexcept = default;

  [[nodiscard]] std::optional<OutputType<Filter<F, I>>> next() {
    for (auto content = underlying.next(); content.has_value(); content = underlying.next()) {
      if (predicate(content.value())) {
        return std::move(content.value());
      }
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
  explicit FlatMap(F func, Iterator<I> &&underlying) : outer(static_cast<I &&>(underlying)), func(func) {
    auto outer_content = outer.next();

    if (outer_content.has_value()) {
      inner = func(std::move(outer_content.value()));
    }
  }
  FlatMap(const FlatMap &) = delete;
  FlatMap(FlatMap &&) noexcept = default;

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
  explicit Take(size_t count, Iterator<I> &&iter) : i(0), take_count(count), underlying(static_cast<I &&>(iter)) {}

  [[nodiscard]] std::optional<OutputType<Take<I>>> next() {
    auto content = underlying.next();

    if (content.has_value() && i < take_count) {
      ++i;
      return content;
    }

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
class Drop : public Iterator<Drop<I>> {
 public:
  explicit Drop(size_t count, Iterator<I> &&iter) : underlying(static_cast<I &&>(iter)) {
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
  explicit Enumerate(Iterator<I> &&iter) : underlying(static_cast<I &&>(iter)), i(0) {}

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

template<typename I>
class Pairwise : public Iterator<Pairwise<I>> {
 public:
  explicit Pairwise(Iterator<I> &&iter) : underlying(static_cast<I&&>(iter)) {
    a = underlying.next();
    b = underlying.next();
  }

  [[nodiscard]] std::optional<OutputType<Pairwise<I>>> next() {
    if (a.has_value() && b.has_value()) {
      auto content = std::make_pair(a.value(), b.value());
      a = b;
      b = underlying.next();

      return content;
    }

    return {};
  }

 private:
  I underlying;
  std::optional<OutputType<I>> a;
  std::optional<OutputType<I>> b;
};

template<typename I>
struct Types<Pairwise<I>> {
  using Output = std::pair<OutputType<I>, OutputType<I>>;
};

template<typename T>
class Range : public Iterator<Range<T>> {
 public:
  explicit Range(T begin, T end) : i(begin), end(end) {}

  [[nodiscard]] std::optional<OutputType<Range>> next() {
    if (i < end) {
      return i++;
    }

    return {};
  }

 private:
  T i;
  T end;
};

template<typename T>
struct Types<Range<T>> {
  using Output = T;
};

}// namespace colex::iterator
