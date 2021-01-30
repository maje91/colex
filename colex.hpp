#pragma once

#include "expressions.hpp"

#include <cstddef>
#include <array>
#include <initializer_list>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <vector>

namespace colex {

/**
 * Creates a map expression. See README for details
 */
template<typename F>
expression::Map<F> map(F func) {
  return expression::Map<F>(func);
}

/**
 * Creates a filter expression. See README for details
 */
template<typename F>
expression::Filter<F> filter(F predicate) {
  return expression::Filter<F>(predicate);
}

/**
 * Creates a fold expression. See README for details
 */
template<typename T, typename F>
expression::Fold<T, F> fold(T initial, F func) {
  return expression::Fold<T, F>(initial, std::move(func));
}

/**
 * Creates a fold1 expression. See README for details
 */
template<typename F>
expression::Fold1<F> fold1(F func) {
  return expression::Fold1<F>(std::move(func));
}

/**
 * Creates a flat map expression. See README for details
 */
template<typename F>
expression::FlatMap<F> flat_map(F func) {
  return expression::FlatMap<F>(func);
}

/**
 * Creates a window expression. See README for details
 */
template<size_t N>
expression::Window<N> window() {
  return expression::Window<N>();
}

/**
 * Creates a take expression. See README for details
 */
expression::Take take(size_t count);

/**
 * Creates a drop expression. See README for details.
 */
expression::Drop drop(size_t count);

/**
 * Creates an enumerate expression. See README for details
 */
expression::Enumerate enumerate();

/**
 * Creates a for each expression. See README for details
 */
template<typename F>
expression::ForEach<F> for_each(F func) {
  return expression::ForEach<F>(func);
}

/**
 * Creates a slice expression. See README for details.
 */
expression::Composition<expression::Drop, expression::Take> slice(size_t start, size_t count);

/**
 * Creates a chunk map expression. See README for details.
 */
template<typename E>
expression::ChunkMap<E> chunk_map(size_t size, expression::Expression<E> &&expr) {
  return expression::ChunkMap<E>(size, std::move(expr));
}

/**
 * Creates a chunk map expression. See README for details.
 */
expression::Chunk chunk(size_t size);

/**
 * Creates a partition expression. See README for details.
 */
expression::Partition partition(std::vector<size_t> partition_sizes);

/**
 * Creates a partition map expression. See README for details.
 */
template<typename E>
expression::PartitionMap<E> partition_map(std::vector<size_t> partition_sizes, expression::Expression<E>&& expr) {
  return expression::PartitionMap<E>(std::move(partition_sizes), std::move(expr));
}

template<typename T>
expression::Prepend<T> prepend(std::vector<T> xs) {
  return expression::Prepend<T>(std::move(xs));
}

template<typename T>
expression::Prepend<T> prepend(std::initializer_list<T> xs) {
  return expression::Prepend<T>(xs);
}

template<typename T>
expression::Append<T> append(std::vector<T> xs) {
  return expression::Append<T>(std::move(xs));
}

template<typename T>
expression::Append<T> append(std::initializer_list<T> xs) {
  return expression::Append<T>(xs);
}

/**
 * Creates a composition of two expressions. `e1` is applied first, then `e2`.
 */
template<typename E1, typename E2>
expression::Composition<E1, E2> operator|(const expression::Expression<E1> &e1, const expression::Expression<E2> &e2) {
  return expression::Composition<E1, E2>(static_cast<const E1&>(e1), static_cast<const E2&>(e2));
}

/**
 * Creates a composition of two expressions. `e1` is applied first, then `e2`.
 */
template<typename E1, typename E2>
expression::Composition<E1, E2> operator|(expression::Expression<E1> &&e1, const expression::Expression<E2> &e2) {
  return expression::Composition<E1, E2>(static_cast<E1&&>(e1), static_cast<const E2&>(e2));
}

/**
 * Creates a composition of two expressions. `e1` is applied first, then `e2`.
 */
template<typename E1, typename E2>
expression::Composition<E1, E2> operator|(const expression::Expression<E1> &e1, expression::Expression<E2> &&e2) {
  return expression::Composition<E1, E2>(static_cast<const E1&>(e1), static_cast<E2&&>(e2));
}

/**
 * Creates a composition of two expressions. `e1` is applied first, then `e2`.
 */
template<typename E1, typename E2>
expression::Composition<E1, E2> operator|(expression::Expression<E1> &&e1, expression::Expression<E2> &&e2) {
  return expression::Composition<E1, E2>(static_cast<E1&&>(e1), static_cast<E2&&>(e2));
}

/**
 * Applies the expression `expr` to the iterator `iter`.
 */
template<typename I, typename E>
expression::OutputType<E, I> operator|(iterator::Iterator<I> &&iter, const expression::Expression<E> &expr) {
  return expr.apply(std::move(iter));
}

/**
 * Creates an iterator from a collection
 */
template<template<typename...> typename C, typename T>
iterator::STL<C, T> iter(const C<T> &collection) {
  return iterator::STL<C, T>(collection);
}

/**
 * Creates an iterator from a collection. The collection is moved
 */
template<template<typename...> typename C, typename T>
iterator::STLMove<C, T> iter(C<T> &&collection) {
  return iterator::STLMove<C, T>(std::move(collection));
}

/**
 * Creates an iterator from an `std::map`.
 */
template<typename K, typename V>
iterator::STLPair<std::map, K, V> iter(const std::map<K, V> &collection) {
  return iterator::STLPair<std::map, K, V>(collection);
}

/**
 * Creates an iterator from an `std::map`. The map is moved.
 */
template<typename K, typename V>
iterator::STLPairMove<std::map, K, V> iter(std::map<K, V> &&collection) {
  return iterator::STLPairMove<std::map, K, V>(std::move(collection));
}

/**
 * Creates an iterator from an `std::unordered_map`.
 */
template<typename K, typename V>
iterator::STLPair<std::unordered_map, K, V> iter(const std::unordered_map<K, V> &collection) {
  return iterator::STLPair<std::unordered_map, K, V>(collection);
}

/**
 * Creates an iterator from an `std::unordered_map`. The map is moved.
 */
template<typename K, typename V>
iterator::STLPairMove<std::unordered_map, K, V> iter(std::unordered_map<K, V> &&collection) {
  return iterator::STLPairMove<std::unordered_map, K, V>(std::move(collection));
}

/**
 * Creates an iterator from an array
 */
template<typename T, size_t N>
iterator::Array<T, N> iter(const std::array<T, N> &collection) {
  return iterator::Array<T, N>(collection);
}

/**
 * Creates an iterator from an array. The array is moved
 */
template<typename T, size_t N>
iterator::ArrayMove<T, N> iter(std::array<T, N> &&collection) {
  return iterator::ArrayMove<T, N>(std::move(collection));
}

/**
 * Creates an iterator from an initializer list.
 */
template<typename T>
iterator::STLMove<std::vector, T> iter(std::initializer_list<T>&& xs) {
  return iterator::STLMove<std::vector, T>(std::move(xs));
}

template<typename T>
iterator::Pointer<T> iter(const T* underlying, size_t element_count) {
  return iterator::Pointer<T>(underlying, element_count);
}

template<typename T>
iterator::PointerMove<T> iter(T*&& underlying, size_t element_count) {
  return iterator::PointerMove<T>(std::move(underlying), element_count);
}

/**
 * Creates a zip iterator from two iterators. See README for details
 */
template<typename I1, typename I2>
iterator::Zip<I1, I2> zip(iterator::Iterator<I1> &&left, iterator::Iterator<I2> &&right) {
  return iterator::Zip<I1, I2>(std::move(left), std::move(right));
}

/**
 * Creates a concat iterator from two iterators. See README for details
 */
template<typename I1, typename I2>
iterator::Concat<I1, I2> concat(iterator::Iterator<I1> &&left, iterator::Iterator<I2> &&right) {
  return iterator::Concat<I1, I2>(std::move(left), std::move(right));
}

/**
 * Creates an iterator over the range `[begin, end)` with step size `step`.
 */
template<typename T>
iterator::Range<T> range(T begin, T end, T step) {
  return iterator::Range(begin, end, step);
}

/**
 * Creates an iterator of the range `[begin, end)` with step size 1
 */
template<typename T>
iterator::Range<T> range(T begin, T end) {
  return iterator::Range(begin, end, T(1));
}

/**
 * Creates an iterator over the range `[begin, inf)` with step size `step`.
 */
template<typename T>
iterator::OpenRange<T> open_range(T begin, T step) {
  return iterator::OpenRange<T>(std::move(begin), std::move(step));
}

/**
 * Creates an iterator that calls func over and over.
 * F: () -> std::optional<T>. The iterator is exhausted
 * when func() returns none.
 */
template<typename F>
iterator::Function<F> func(F func) {
  return iterator::Function<F>(std::move(func));
}

/**
 * Base type for things iterators can be collected into
 */
template<template<typename...> typename C>
struct collect;

template<>
struct collect<std::vector> {};

/**
 * Collects an iterator into an `std::vector`.
 */
template<typename I>
std::vector<iterator::OutputType<I>> operator|(iterator::Iterator<I> &&iter, collect<std::vector> &&) {
  std::vector<iterator::OutputType<I>> result;

  for (auto content = iter.next(); content.has_value(); content = iter.next()) {
    result.push_back(std::move(content.value()));
  }

  return std::move(result);
}

template<>
struct collect<std::set> {};

/**
 * Collects an iterator into an `std::set`.
 */
template<typename I>
std::set<iterator::OutputType<I>> operator|(iterator::Iterator<I> &&iter, collect<std::set> &&) {
  std::set<iterator::OutputType<I>> result;

  for (auto content = iter.next(); content.has_value(); content = iter.next()) {
    result.insert(std::move(content.value()));
  }

  return std::move(result);
}

template<>
struct collect<std::unordered_set> {};

/**
 * Collects an iterator into an `std::unordered_set`.
 */
template<typename I>
std::unordered_set<iterator::OutputType<I>> operator|(iterator::Iterator<I> &&iter, collect<std::unordered_set> &&) {
  std::unordered_set<iterator::OutputType<I>> result;

  for (auto content = iter.next(); content.has_value(); content = iter.next()) {
    result.insert(std::move(content.value()));
  }

  return std::move(result);
}

template<>
struct collect<std::map> {};

/**
 * Collects an iterator into an `std::map`.
 */
template<typename I>
std::map<typename iterator::OutputType<I>::first_type, typename iterator::OutputType<I>::second_type>
        operator|(iterator::Iterator<I> &&iter, collect<std::map> &&) {
  std::map<typename iterator::OutputType<I>::first_type, typename iterator::OutputType<I>::second_type> result;

  for (auto content = iter.next(); content.has_value(); content = iter.next()) {
    result.insert(std::move(content.value()));
  }

  return std::move(result);
}

template<>
struct collect<std::unordered_map> {};

/**
 * Collects an iterator into an `std::unordered_map`.
 */
template<typename I>
std::unordered_map<typename iterator::OutputType<I>::first_type, typename iterator::OutputType<I>::second_type>
operator|(iterator::Iterator<I> &&iter, collect<std::unordered_map> &&) {
  std::unordered_map<typename iterator::OutputType<I>::first_type, typename iterator::OutputType<I>::second_type> result;

  for (auto content = iter.next(); content.has_value(); content = iter.next()) {
    result.insert(std::move(content.value()));
  }

  return std::move(result);
}

}// namespace colex
