#pragma once

#include <optional>

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

}// namespace colex::iterator
