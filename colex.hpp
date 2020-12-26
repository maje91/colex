#pragma once

#include "expressions.hpp"

#include <cstddef>
#include <array>

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
 * Creates a flat map expression. See README for details
 */
template<typename F>
expression::FlatMap<F> flat_map(F func) {
  return expression::FlatMap<F>(func);
}

expression::Take take(size_t count);

/**
 * Creates a composition of two expressions. `e1` is applied first, then `e2`.
 */
template<typename E1, typename E2>
expression::Composition<E1, E2> operator|(expression::Expression<E1> e1, expression::Expression<E2> e2) {
  return expression::Composition<E1, E2>(std::move(e1), std::move(e2));
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
 * Base type for things iterators can be collected into
 */
template<template<typename...> typename C>
struct collect;

template<>
struct collect<std::vector> {};

template<>
struct collect<std::set> {};

/**
 * Collects an iterator into an `std::vector`.
 */
template<typename I>
std::vector<iterator::OutputType<I>> operator|(iterator::Iterator<I> &&iter, collect<std::vector> &&) {
  std::vector<iterator::OutputType<I>> result;

  for (;!iter.at_end(); iter.advance()) {
    result.push_back(std::move(iter.content()));
  }

  return std::move(result);
}

/**
 * Collects an iterator into an `std::set`.
 */
template<typename I>
std::set<iterator::OutputType<I>> operator|(iterator::Iterator<I> &&iter, collect<std::set> &&) {
  std::set<iterator::OutputType<I>> result;

  for (;!iter.at_end(); iter.advance()) {
    result.insert(std::move(iter.content()));
  }

  return std::move(result);
}

}// namespace colex