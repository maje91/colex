#pragma once

#include "../inc/interface.hpp"

#include <set>
#include <array>

namespace colex::iterator {

/**
 * An iterator over a borrowed STL collection
 */
template<template<typename...> typename C, typename T>
class STL : public Iterator<STL<C, T>> {
 public:
  explicit STL(const C<T> &underlying)
          : it(underlying.begin()), end(underlying.end()) {}

  STL(const STL &) = delete;
  STL(STL &&) noexcept = default;
  STL &operator=(STL &&) noexcept = default;
  STL &operator=(const STL &) = delete;

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

  STLMove(const STLMove &) = delete;
  STLMove(STLMove &&) noexcept = default;
  STLMove &operator=(STLMove &&) noexcept = default;
  STLMove &operator=(const STLMove &) = delete;

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

  STLMove(const STLMove &) = delete;
  STLMove(STLMove &&) noexcept = default;
  STLMove &operator=(STLMove &&) noexcept = default;
  STLMove &operator=(const STLMove &) = delete;

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

  STLPair(const STLPair &) = delete;
  STLPair(STLPair &&) noexcept = default;
  STLPair &operator=(STLPair &&) noexcept = default;
  STLPair &operator=(const STLPair &) = delete;

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

  STLPairMove(const STLPairMove &) = delete;
  STLPairMove(STLPairMove &&) noexcept = default;
  STLPairMove &operator=(STLPairMove &&) noexcept = default;
  STLPairMove &operator=(const STLPairMove &) = delete;

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

  Array(const Array &) = delete;
  Array(Array &&) noexcept = default;
  Array &operator=(Array &&) noexcept = default;
  Array &operator=(const Array &) = delete;

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

  ArrayMove(const ArrayMove &) = delete;
  ArrayMove(ArrayMove &&) noexcept = default;
  ArrayMove &operator=(ArrayMove &&) noexcept = default;
  ArrayMove &operator=(const ArrayMove &) = delete;

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

}
