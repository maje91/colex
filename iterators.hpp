#pragma once

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
   * Returns true if there are no more element to iterate over
   */
  [[nodiscard]] bool at_end() const {
    return static_cast<const I &>(*this).at_end();
  }

  /**
   * Returns the next item. Must not be called if `at_end() == true`.
   */
  [[nodiscard]] OutputType<I> next() {
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

  [[nodiscard]] bool at_end() const {
    return it == end;
  }

  [[nodiscard]] OutputType<STL<C, T>> next() {
    return *(it++);
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

  [[nodiscard]] bool at_end() const {
    return it == underlying.end();
  }

  [[nodiscard]] OutputType<STLMove<C, T>> next() {
    return std::move(*(it++));
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

  [[nodiscard]] bool at_end() const {
    return underlying.empty();
  }

  [[nodiscard]] OutputType<STLMove<std::set, T>> next() {
    return std::move(underlying.extract(underlying.begin()).value());
  }

 private:
  std::set<T> underlying;
};

template<template<typename...> typename C, typename T>
struct Types<STLMove<C, T>> {
  using Output = T;
};

template<typename T, size_t N>
class Array : public Iterator<Array<T, N>> {
 public:
  explicit Array(const std::array<T, N> &underlying) : i(0), underlying(underlying) {}

  [[nodiscard]] bool at_end() const {
    return i == N;
  }

  [[nodiscard]] OutputType<Array<T, N>> next() {
    return underlying[i++];
  }

 private:
  size_t i;
  const std::array<T, N> &underlying;
};

template<typename T, size_t N>
struct Types<Array<T, N>> {
  using Output = const T&;
};

template<typename T, size_t N>
class ArrayMove : public Iterator<ArrayMove<T, N>> {
 public:
  explicit ArrayMove(std::array<T, N> &&underlying) : i(0), underlying(std::move(underlying)) {}

  [[nodiscard]] bool at_end() const {
    return i == N;
  }

  [[nodiscard]] OutputType<ArrayMove<T, N>> next() {
    return std::move(underlying[i++]);
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

  [[nodiscard]] bool at_end() const {
    return underlying.at_end();
  }

  [[nodiscard]] OutputType<Map<F, I>> next() {
    return func(underlying.next());
  }

 private:
  I underlying;
  F func;
};

template<typename F, typename I>
struct Types<Map<F, I>> {
  using Output = std::invoke_result_t<F, OutputType<I>>;
};

}// namespace colex::iterator
