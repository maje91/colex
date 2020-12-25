#pragma once

#include "iterators.hpp"

namespace colex::expression {

/**
 * Base for types associated with an expression.
 * Must be specialized for each expression.
 * The aliases just below specifies which
 * types that must be defined.
 */
template<typename E, typename I>
struct Types;

/**
 * The type output after applying an expression
 */
template<typename E, typename I>
using OutputType = typename Types<E, I>::Output;

/**
 * Base for all expressions
 */
template<typename E>
struct Expression {
  /**
   * Apply the expression to an iterator
   */
  template<typename I>
  OutputType<E, I> apply(iterator::Iterator<I> &&iter) const {
    return static_cast<const E &>(*this).apply(std::move(iter));
  }
};

template<typename F>
class Map : public Expression<Map<F>> {
 public:
  explicit Map(F func) : func(func) {}

  template<typename I>
  OutputType<Map<F>, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Map<F, I>(func, std::move(iter));
  }

 private:
  F func;
};

template<typename F, typename I>
struct Types<Map<F>, I> {
  using Output = iterator::Map<F, I>;
};

template<typename T, typename F>
class Fold : public Expression<Fold<T, F>> {
 public:
  explicit Fold(T initial, F func) : func(std::move(func)), initial(std::move(initial)) {}

  template<typename I>
  T apply(iterator::Iterator<I> &&iter) const {
    T result = initial;

    for (;!iter.at_end(); result = func(std::move(result), iter.next()));

    return result;
  }

 private:
  F func;
  T initial;
};

template<typename T, typename F, typename I>
struct Types<Fold<T, F>, I> {
  using Output = T;
};

template<typename E1, typename E2>
class Composition {
 public:
  explicit Composition(Expression<E1> e1, Expression<E2> e2) : e1(static_cast<E1 &&>(e1)), e2(static_cast<E2 &&>(e2)) {}

  template<typename I>
  OutputType<Composition<E1, E2>, I> apply(iterator::Iterator<I> &&iter) const {
    e2.apply(e1.apply(std::move(iter)));
  }

 private:
  E1 e1;
  E2 e2;
};

template<typename E1, typename E2, typename I>
struct Types<Composition<E1, E2>, I> {
  using Output = OutputType<E2, I>;
};

}// namespace colex::expression
