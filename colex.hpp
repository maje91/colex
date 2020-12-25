#pragma once

#include "expressions.hpp"

namespace colex {

/**
 * Creates a map expression. See README for details
 * ```
 */
template<typename F>
expression::Map<F> map(F func) {
  return expression::Map<F>(func);
}

/**
 * Creates a fold expression. See README for details
 */
template<typename T, typename F>
expression::Fold<T, F> fold(T initial, F func) {
  return expression::Fold<T, F>(initial, std::move(func));
}

/**
 * Creates a composition of two expressions. `e1` is applied first, then `e2`.
 */
template<typename E1, typename E2>
expression::Composition<E1, E2> operator|(expression::Expression<E1> e1, expression::Expression<E2> e2) {
  return expression::Composition<E1, E2>(std::move(e1), std::move(e2));
}

/**
 * Applies the expression `expr` to the collection `xs`.
 */
template<template<typename...> typename C, typename T, typename E>
expression::OutputType<E, iterator::STL<C, T>> operator|(const C<T> &xs, const expression::Expression<E> &expr) {
  return expr.apply(iterator::STL<C, T>(xs));
}

/**
 * Applies the expression `expr` to the collection `xs`. `xs` is moved.
 */
template<template<typename...> typename C, typename T, typename E>
expression::OutputType<E, iterator::STLMove<C, T>> operator|(C<T> &&xs, const expression::Expression<E> &expr) {
  return expr.apply(iterator::STLMove<C, T>(std::move(xs)));
}

/**
 * Applies the expression `expr` to the array `xs`.
 */
template<typename T, size_t N, typename E>
expression::OutputType<E, iterator::Array<T, N>> operator|(const std::array<T, N> &xs, const expression::Expression<E> &expr) {
  return expr.apply(iterator::Array<T, N>(xs));
}

/**
 * Applies the expression `expr` to the array `xs`. `xs` is moved.
 */
template<typename T, size_t N, typename E>
expression::OutputType<E, iterator::Array<T, N>> operator|(std::array<T, N> &&xs, const expression::Expression<E> &expr) {
  return expr.apply(iterator::ArrayMove<T, N>(std::move(xs)));
}

/**
 * Applies the expression `expr` to the iterator `iter`.
 */
template<typename I, typename E>
expression::OutputType<E, I> operator|(iterator::Iterator<I> &&iter, const expression::Expression<E> &expr) {
  return expr.apply(std::move(iter));
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
 * Converts a collection of type `C2` to a collection of type `C1`.
 */
template<template<typename...> typename C1, template<typename...> typename C2, typename T>
C1<T> operator|(const C2<T> &collection, collect<C1>&&) {
  return iterator::STL<C2, T>(collection) | collect<C1>();
}

/**
 * Converts a collection of type `C2` to a collection of type `C1`. `C2` is moved.
 */
template<template<typename...> typename C1, template<typename...> typename C2, typename T>
C1<T> operator|(C2<T> &&collection, collect<C1>&&) {
  return iterator::STLMove<C2, T>(std::move(collection)) | collect<C1>();
}

/**
 * Converts an array to a collection of type `C`.
 */
template<template<typename...> typename C, typename T, size_t N>
C<T> operator|(const std::array<T, N> &array, collect<C>&&) {
  return iterator::Array<T, N>(array) | collect<C>();
}

/**
 * Converts an array to a collection of type `C`. The array is moved.
 */
template<template<typename...> typename C, typename T, size_t N>
C<T> operator|(std::array<T, N> &&array, collect<C>&&) {
  return iterator::ArrayMove<T, N>(std::move(array)) | collect<C>();
}

/**
 * Collects an iterator into an `std::vector`.
 */
template<typename I>
std::vector<iterator::OutputType<I>> operator|(iterator::Iterator<I> &&iter, collect<std::vector> &&) {
  std::vector<iterator::OutputType<I>> result;

  for (;!iter.at_end(); result.push_back(std::move(iter.next())));

  return std::move(result);
}

/**
 * Collects an iterator into an `std::set`.
 */
template<typename I>
std::set<iterator::OutputType<I>> operator|(iterator::Iterator<I> &&iter, collect<std::set> &&) {
  std::set<iterator::OutputType<I>> result;

  for (;!iter.at_end(); result.insert(std::move(iter.next())));

  return std::move(result);
}

}// namespace colex