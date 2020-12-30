#pragma once

#include "iterator_interface.hpp"

#include <utility>

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

}// namespace colex::expression
