#pragma once

#include <utility>
#include <set>

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
 * Base for all iterators. To consume an iterator
 * the functions must be called in this order:
 * `at_end`, `content`, `advance`. Repeat until
 * `at_end` is true. All three functions are allowed
 * to modify internal state, and implementations assume that
 * the call order is adhered to. As an example, this for loop is
 * valid
 * ```cpp
 * for (; iter.at_end(); iter.advance()) {
 *   auto x = iter.content();
 *   // Do something with x
 * }
 * ```
 *
 * The docstrings for the functions in `Iterator` describe their
 * intended meanings, and might not necessarily be true for all
 * implementations.
 */
template<typename I>
struct Iterator {
  /**
   * Returns true if there are no more element to iterate over.
   */
  [[nodiscard]] bool at_end() {
    return static_cast<I &>(*this).at_end();
  }

  /**
   * Returns the current item. Must not be called if `at_end() == true`.
   */
  [[nodiscard]] OutputType<I> content() {
    return static_cast<I &>(*this).content();
  }

  /**
   * Advances the iterator.
   */
  void advance() {
    static_cast<I &>(*this).advance();
  }
};

/**
 * An iterator over a borrowed STL collection
 */
template<template<typename...> typename C, typename T>
class STL : public Iterator<STL<C, T>> {
 public:
  explicit STL(const C<T> &underlying) : it(underlying.begin()), end(underlying.end()) {}

  [[nodiscard]] bool at_end() {
    return it == end;
  }

  [[nodiscard]] OutputType<STL<C, T>> content() {
    return *it;
  }

  void advance() {
    ++it;
  }

 private:
  typename C<T>::const_iterator it;
  typename C<T>::const_iterator end;
};

template<template<typename...> typename C, typename T>
struct Types<STL<C, T>> {
  using Output = const T &;
};

/**
 * An iterator over an owned STL collection
 */
template<template<typename...> typename C, typename T>
struct STLMove : public Iterator<STLMove<C, T>> {
 public:
  explicit STLMove(C<T> &&underlying) : it(underlying.begin()), underlying(std::move(underlying)) {}

  [[nodiscard]] bool at_end() {
    return it == underlying.end();
  }

  [[nodiscard]] OutputType<STLMove<C, T>> content() {
    return std::move(*it);
  }

  void advance() {
    ++it;
  }

 private:
  typename C<T>::iterator it;
  C<T> underlying;
};

/**
 * `std::set` requires special treatment
 */
template<typename T>
struct STLMove<std::set, T> : public Iterator<STLMove<std::set, T>> {
 public:
  explicit STLMove(std::set<T> &&underlying) : underlying(std::move(underlying)) {}

  [[nodiscard]] bool at_end() {
    return underlying.empty();
  }

  [[nodiscard]] OutputType<STLMove<std::set, T>> content() {
    return std::move(underlying.extract(underlying.begin()).value());
  }

  void advance() {}

 private:
  std::set<T> underlying;
};

template<template<typename...> typename C, typename T>
struct Types<STLMove<C, T>> {
  using Output = T;
};

/**
 * An iterator over a borrowed array
 */
template<typename T, size_t N>
class Array : public Iterator<Array<T, N>> {
 public:
  explicit Array(const std::array<T, N> &underlying) : i(0), underlying(underlying) {}

  [[nodiscard]] bool at_end() {
    return i == N;
  }

  [[nodiscard]] OutputType<Array<T, N>> content() {
    return underlying[i];
  }

  void advance() {
    ++i;
  }

 private:
  size_t i;
  const std::array<T, N> &underlying;
};

template<typename T, size_t N>
struct Types<Array<T, N>> {
  using Output = const T &;
};

/**
 * An iterator over an owned array
 */
template<typename T, size_t N>
class ArrayMove : public Iterator<ArrayMove<T, N>> {
 public:
  explicit ArrayMove(std::array<T, N> &&underlying) : i(0), underlying(std::move(underlying)) {}

  [[nodiscard]] bool at_end() {
    return i == N;
  }

  [[nodiscard]] OutputType<ArrayMove<T, N>> content() {
    return std::move(underlying[i]);
  }

  void advance() {
    ++i;
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

  [[nodiscard]] bool at_end() {
    return underlying.at_end();
  }

  [[nodiscard]] OutputType<Map<F, I>> content() {
    return func(underlying.content());
  }

  void advance() {
    underlying.advance();
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

  [[nodiscard]] bool at_end() {
    while (!underlying.at_end()) {
      if (predicate(content())) { break; }
      advance();
    }

    return underlying.at_end();
  }

  [[nodiscard]] OutputType<Filter<F, I>> content() {
    return underlying.content();
  }

  void advance() {
    return underlying.advance();
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
          : outer(static_cast<I &&>(underlying))
          , inner(func(outer.content()))
          , func(func)
  {}
  FlatMap(const FlatMap &) = delete;
  FlatMap(FlatMap &&) noexcept = default;

  [[nodiscard]] bool at_end() {
    return outer.at_end();
  }

  [[nodiscard]] OutputType<FlatMap<F, I>> content() {
    return inner.content();
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
  std::invoke_result_t<F, OutputType<I>> inner;
  F func;
};

template<typename F, typename I>
struct Types<FlatMap<F, I>> {
  using Output = OutputType<std::invoke_result_t<F, OutputType<I>>>;
};

template<typename I>
class Take : public Iterator<Take<I>> {
 public:
  explicit Take(size_t count, Iterator<I>&& iter) : i(0), take_count(count), underlying(static_cast<I&&>(iter)) {}

  [[nodiscard]] bool at_end() {
    return underlying.at_end() || i == take_count;
  }

  [[nodiscard]] OutputType<Take<I>> content() {
    return underlying.content();
  }

  void advance() {
    underlying.advance();
    ++i;
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
  explicit Drop(size_t count, Iterator<I>&& iter) : underlying(static_cast<I&&>(iter)) {
    for (size_t i = 0; i < count && !underlying.at_end(); underlying.advance(), ++i);
  }

  [[nodiscard]] bool at_end() {
    return underlying.at_end();
  }

  [[nodiscard]] OutputType<Drop<I>> content() {
    return underlying.content();
  }

  void advance() {
    underlying.advance();
  }

 private:
  I underlying;
};

template<typename I>
struct Types<Drop<I>> {
  using Output = OutputType<I>;
};

}// namespace colex::iterator
