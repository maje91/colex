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

template<typename F>
class Filter : public Expression<Filter<F>> {
 public:
  explicit Filter(F predicate) : predicate(predicate) {}

  template<typename I>
  OutputType<Filter<F>, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Filter<F, I>(predicate, std::move(iter));
  }

 private:
  F predicate;
};

template<typename F, typename I>
struct Types<Filter<F>, I> {
  using Output = iterator::Filter<F, I>;
};

template<typename T, typename F>
class Fold : public Expression<Fold<T, F>> {
 public:
  explicit Fold(T initial, F func) : func(std::move(func)), initial(std::move(initial)) {}

  template<typename I>
  T apply(iterator::Iterator<I> &&iter) const {
    T result = initial;

    for (auto content = iter.next() ; content.has_value(); content = iter.next()) {
      result = func(std::move(result), std::move(content.value()));
    }

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

template<typename F>
class FlatMap : public Expression<FlatMap<F>> {
 public:
  explicit FlatMap(F func) : func(func) {}

  template<typename I>
  OutputType<FlatMap<F>, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::FlatMap<F, I>(func, std::move(iter));
  }

 private:
  F func;
};

template<typename F, typename I>
struct Types<FlatMap<F>, I> {
  using Output = iterator::FlatMap<F, I>;
};

class Take : public Expression<Take> {
 public:
  explicit Take(size_t count) : count(count) {}

  template<typename I>
  OutputType<Take, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Take<I>(count, std::move(iter));
  }

 private:
  size_t count;
};

template<typename I>
struct Types<Take, I> {
  using Output = iterator::Take<I>;
};

class Drop : public Expression<Drop> {
 public:
  explicit Drop(size_t count) : count(count) {}

  template<typename I>
  OutputType<Drop, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Drop<I>(count, std::move(iter));
  }

 private:
  size_t count;
};

template<typename I>
struct Types<Drop, I> {
  using Output = iterator::Drop<I>;
};

class Enumerate : public Expression<Enumerate> {
 public:
  explicit Enumerate() {}

  template<typename I>
  OutputType<Enumerate, I> apply(iterator::Iterator<I> &&iter) const {
    return iterator::Enumerate<I>(std::move(iter));
  }
};

template<typename I>
struct Types<Enumerate, I> {
  using Output = iterator::Enumerate<I>;
};

template<typename F>
class ForEach : public Expression<ForEach<F>> {
 public:
  explicit ForEach(F func) : func(func) {}

  template<typename I>
  OutputType<ForEach<F>, I> apply(iterator::Iterator<I> &&iter) const {
    for (auto content = iter.next(); content.has_value(); content = iter.next()) {
      func(std::move(content.value()));
    }
  }

 private:
  F func;
};

template<typename F, typename I>
struct Types<ForEach<F>, I> {
  using Output = void;
};

template<typename E1, typename E2>
class Composition : public Expression<Composition<E1, E2>> {
 public:
  explicit Composition(const E1 &e1, const E2 &e2) : e1(e1), e2(e2) {}
  explicit Composition(E1&& e1, const E2 &e2) : e1(std::move(e1)), e2(e2) {}
  explicit Composition(const E1 &e1, E2&& e2) : e1(e1), e2(std::move(e2)) {}
  explicit Composition(E1&& e1, E2&& e2) : e1(std::move(e1)), e2(std::move(e2)) {}

  template<typename I>
  OutputType<Composition<E1, E2>, I> apply(iterator::Iterator<I> &&iter) const {
    return e2.apply(e1.apply(std::move(iter)));
  }

 private:
  E1 e1;
  E2 e2;
};

template<typename E1, typename E2, typename I>
struct Types<Composition<E1, E2>, I> {
  using Output = OutputType<E2, OutputType<E1, I>>;
};

}// namespace colex::expression
