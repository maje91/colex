#pragma once

#include "expression_interface.hpp"
#include "iterator_interface.hpp"

#include <array>
#include <functional>
#include <set>
#include <utility>

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

template<typename T>
class Pointer : public Iterator<Pointer<T>> {
 public:
  explicit Pointer(const T *underlying, size_t element_count)
      : m_ptr(underlying), m_end(m_ptr + element_count) {}

  Pointer(const Pointer &) = delete;
  Pointer(Pointer &&) noexcept = default;
  Pointer &operator=(Pointer &&) noexcept = default;
  Pointer &operator=(const Pointer &) = delete;

  [[nodiscard]] std::optional<OutputType<Pointer<T>>> next() {
    if (m_ptr != m_end) { return *(m_ptr++); }

    return {};
  }

 private:
  const T *m_ptr;
  const T *m_end;
};

template<typename T>
struct Types<Pointer<T>> {
  using Output = T;
};

template<typename T>
class PointerMove : public Iterator<PointerMove<T>> {
 public:
  explicit PointerMove(T *underlying, size_t element_count)
      : m_ptr(underlying), m_end(m_ptr + element_count) {}

  PointerMove(const PointerMove &) = delete;
  PointerMove(PointerMove &&) noexcept = default;
  PointerMove &operator=(PointerMove &&) noexcept = default;
  PointerMove &operator=(const PointerMove &) = delete;

  [[nodiscard]] std::optional<OutputType<PointerMove<T>>> next() {
    if (m_ptr != m_end) { return std::move(*(m_ptr++)); }

    return {};
  }

 private:
  T *m_ptr;
  T *m_end;
};

template<typename T>
struct Types<PointerMove<T>> {
  using Output = T;
};

template<typename F, typename I>
class Map : public Iterator<Map<F, I>> {
 public:
  explicit Map(F func, Iterator<I> &&underlying)
      : underlying(static_cast<I &&>(underlying)), func(func) {}

  Map(const Map &) = delete;
  Map(Map &&) noexcept = default;
  Map &operator=(Map &&) noexcept = default;
  Map &operator=(const Map &) = delete;

  [[nodiscard]] std::optional<OutputType<Map<F, I>>> next() {
    auto content = underlying.next();

    if (content.has_value()) { return func(std::move(content.value())); }

    return {};
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
  explicit Filter(F predicate, Iterator<I> &&underlying)
      : underlying(static_cast<I &&>(underlying)), predicate(predicate) {}

  Filter(const Filter &) = delete;
  Filter(Filter &&) noexcept = default;
  Filter &operator=(Filter &&) noexcept = default;
  Filter &operator=(const Filter &) = delete;

  [[nodiscard]] std::optional<OutputType<Filter<F, I>>> next() {
    for (auto content = underlying.next(); content.has_value();
         content = underlying.next()) {
      if (predicate(content.value())) { return std::move(content.value()); }
    }

    return {};
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
      : m_outer(static_cast<I &&>(underlying)), m_func(std::move(func)) {
    auto outer_content = m_outer.next();

    if (outer_content.has_value()) {
      m_inner.emplace(m_func(std::move(outer_content.value())));
    }
  }

  FlatMap(const FlatMap &) = delete;
  FlatMap(FlatMap &&) noexcept = default;
  FlatMap &operator=(FlatMap &&) noexcept = default;
  FlatMap &operator=(const FlatMap &) = delete;

  [[nodiscard]] std::optional<OutputType<FlatMap<F, I>>> next() {
    if (m_inner.has_value()) {
      auto inner_content = m_inner.value().next();

      if (inner_content.has_value()) { return inner_content; }

      auto outer_content = m_outer.next();

      if (outer_content.has_value()) {
        m_inner.emplace(m_func(std::move(outer_content.value())));
        return next();
      }
    }

    return {};
  }

 private:
  I m_outer;
  std::optional<std::invoke_result_t<F, OutputType<I>>> m_inner;
  F m_func;
};

template<typename F, typename I>
struct Types<FlatMap<F, I>> {
  using Output = OutputType<std::invoke_result_t<F, OutputType<I>>>;
};

template<typename I>
class Take : public Iterator<Take<I>> {
 public:
  explicit Take(size_t count, Iterator<I> &&iter)
      : i(0), take_count(count), underlying(static_cast<I &&>(iter)) {}

  Take(const Take &) = delete;
  Take(Take &&) noexcept = default;
  Take &operator=(Take &&) noexcept = default;
  Take &operator=(const Take &) = delete;

  [[nodiscard]] std::optional<OutputType<Take<I>>> next() {
    if (i++ < take_count) { return underlying.next(); }

    return {};
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
class TakeRef : public Iterator<TakeRef<I>> {
 public:
  explicit TakeRef(size_t count, Iterator<I> &iter)
      : i(0), take_count(count), underlying(static_cast<I &>(iter)) {}

  TakeRef(const TakeRef &) = delete;
  TakeRef(TakeRef &&) noexcept = default;
  TakeRef &operator=(TakeRef &&) noexcept = default;
  TakeRef &operator=(const TakeRef &) = delete;

  [[nodiscard]] std::optional<OutputType<TakeRef<I>>> next() {
    if (i++ < take_count) { return underlying.next(); }

    return {};
  }

 private:
  size_t i;
  size_t take_count;
  I &underlying;
};

template<typename I>
struct Types<TakeRef<I>> {
  using Output = OutputType<I>;
};

template<typename I>
class Drop : public Iterator<Drop<I>> {
 public:
  explicit Drop(size_t count, Iterator<I> &&iter)
      : underlying(static_cast<I &&>(iter)) {
    for (size_t i = 0; i < count; ++i) {
      if (!underlying.next().has_value()) { break; }
    }
  }

  Drop(const Drop &) = delete;
  Drop(Drop &&) noexcept = default;
  Drop &operator=(Drop &&) noexcept = default;
  Drop &operator=(const Drop &) = delete;

  [[nodiscard]] std::optional<OutputType<Drop<I>>> next() {
    return underlying.next();
  }

 private:
  I underlying;
};

template<typename I>
struct Types<Drop<I>> {
  using Output = OutputType<I>;
};

template<typename I>
class Enumerate : public Iterator<Enumerate<I>> {
 public:
  explicit Enumerate(Iterator<I> &&iter)
      : underlying(static_cast<I &&>(iter)), i(0) {}

  Enumerate(const Enumerate &) = delete;
  Enumerate(Enumerate &&) noexcept = default;
  Enumerate &operator=(Enumerate &&) noexcept = default;
  Enumerate &operator=(const Enumerate &) = delete;

  [[nodiscard]] std::optional<OutputType<Enumerate<I>>> next() {
    auto content = underlying.next();

    if (content.has_value()) {
      return std::make_pair(i++, std::move(content.value()));
    }

    return {};
  }

 private:
  I underlying;
  size_t i;
};

template<typename I>
struct Types<Enumerate<I>> {
  using Output = std::pair<size_t, OutputType<I>>;
};

template<size_t N, typename I>
class Window : public Iterator<Window<N, I>> {
 public:
  explicit Window(Iterator<I> &&iter)
      : m_underlying(static_cast<I &&>(iter)), m_start_index(0) {
    for (size_t i = 0; i < N; ++i) { m_elements[i] = m_underlying.next(); }
  }

  Window(const Window &) = delete;
  Window(Window &&) noexcept = default;
  Window &operator=(Window &&) noexcept = default;
  Window &operator=(const Window &) = delete;

  [[nodiscard]] bool last_is_none() const {
    if (m_start_index == 0) { return !m_elements[N - 1].has_value(); }

    return !m_elements[m_start_index - 1].has_value();
  }

  [[nodiscard]] std::optional<OutputType<Window<N, I>>> next() {
    if (last_is_none()) { return {}; }

    std::array<OutputType<I>, N> content;
    for (size_t i = 0; i < N; ++i) {
      size_t j = (m_start_index + i) % N;
      content[i] = m_elements[j].value();
    }

    m_elements[m_start_index] = m_underlying.next();
    m_start_index = (m_start_index + 1) % N;

    return std::move(content);
  }

 private:
  I m_underlying;
  size_t m_start_index;
  std::array<std::optional<OutputType<I>>, N> m_elements;
};

template<size_t N, typename I>
struct Types<Window<N, I>> {
  using Output = std::array<OutputType<I>, N>;
};

template<typename T>
class Range : public Iterator<Range<T>> {
 public:
  explicit Range(T begin, T end, T step) : i(begin), end(end), step(step) {}

  Range(const Range &) = delete;
  Range(Range &&) noexcept = default;
  Range &operator=(Range &&) noexcept = default;
  Range &operator=(const Range &) = delete;

  [[nodiscard]] std::optional<OutputType<Range>> next() {
    if (i < end) {
      T value = i;
      i += step;

      return value;
    }

    return {};
  }

 private:
  T i;
  T end;
  T step;
};

template<typename T>
struct Types<Range<T>> {
  using Output = T;
};


template<typename T>
class OpenRange : public Iterator<OpenRange<T>> {
 public:
  explicit OpenRange(T begin, T step) : i(begin), step(step) {}

  OpenRange(const OpenRange &) = delete;
  OpenRange(OpenRange &&) noexcept = default;
  OpenRange &operator=(OpenRange &&) noexcept = default;
  OpenRange &operator=(const OpenRange &) = delete;

  [[nodiscard]] std::optional<OutputType<OpenRange>> next() {
    T value = i;
    i += step;

    return value;
  }

 private:
  T i;
  T step;
};

template<typename T>
struct Types<OpenRange<T>> {
  using Output = T;
};

template<typename F>
class Function : public Iterator<Function<F>> {
 public:
  explicit Function(F func) : m_func(std::move(func)) {}

  Function(const Function &) = delete;
  Function(Function &&) noexcept = default;
  Function &operator=(Function &&) noexcept = default;
  Function &operator=(const Function &) = delete;

  [[nodiscard]] std::optional<OutputType<Function<F>>> next() {
    return std::move(m_func());
  }

 private:
  F m_func;
};

template<typename F>
struct Types<Function<F>> {
  using Output = typename std::result_of_t<F()>::value_type;
};

template<typename I1, typename I2,
         std::enable_if_t<std::is_same_v<OutputType<I1>, OutputType<I2>>, int> =
                 0>
class Concat : public Iterator<Concat<I1, I2>> {
 public:
  explicit Concat(Iterator<I1> &&left, Iterator<I2> &&right)
      : left(static_cast<I1 &&>(left)), right(static_cast<I2 &&>(right)) {}

  Concat(const Concat &) = delete;
  Concat(Concat &&) noexcept = default;
  Concat &operator=(Concat &&) noexcept = default;
  Concat &operator=(const Concat &) = delete;

  [[nodiscard]] std::optional<OutputType<Concat<I1, I2>>> next() {
    auto left_content = left.next();

    if (left_content.has_value()) { return std::move(left_content.value()); }

    return right.next();
  }

 private:
  I1 left;
  I2 right;
};

template<typename I1, typename I2>
struct Types<Concat<I1, I2>> {
  using Output = OutputType<I1>;
};

template<typename E, typename I>
class ChunkMap : public Iterator<ChunkMap<E, I>> {
 public:
  explicit ChunkMap(size_t size, E expr, Iterator<I> &&iter)
      : underlying(static_cast<I &&>(iter)), size(size), expr(std::move(expr)) {}

  ChunkMap(const ChunkMap &) = delete;
  ChunkMap(ChunkMap &&) noexcept = default;
  ChunkMap &operator=(ChunkMap &&) noexcept = default;
  ChunkMap &operator=(const ChunkMap &) = delete;

  [[nodiscard]] std::optional<OutputType<ChunkMap<E, I>>> next() {
    auto it = TakeRef<I>(size, underlying);
    auto first = it.next();
    if (first.has_value()) {
      return expr.apply(Concat(ArrayMove(std::array{std::move(first.value())}),
                               std::move(it)));
    }

    return {};
  }

 private:
  I underlying;
  size_t size;
  E expr;
};

template<typename E, typename I>
struct Types<ChunkMap<E, I>> {
  using Output = expression::OutputType<
          E, Concat<ArrayMove<OutputType<I>, 1>, TakeRef<I>>>;
};

template<typename I>
class Chunk : public Iterator<Chunk<I>> {
 public:
  explicit Chunk(size_t size, Iterator<I> &&iter)
      : underlying(static_cast<I &&>(iter)), size(size) {}

  Chunk(const Chunk &) = delete;
  Chunk(Chunk &&) noexcept = default;
  Chunk &operator=(Chunk &&) noexcept = default;
  Chunk &operator=(const Chunk &) = delete;

  [[nodiscard]] std::optional<OutputType<Chunk<I>>> next() {
    auto it = TakeRef<I>(size, underlying);
    auto first = it.next();
    if (first.has_value()) {
      return Concat(ArrayMove(std::array{std::move(first.value())}),
                    std::move(it));
    }

    return {};
  }

 private:
  I underlying;
  size_t size;
};

template<typename I>
struct Types<Chunk<I>> {
  using Output = Concat<ArrayMove<OutputType<I>, 1>, TakeRef<I>>;
};

template<typename I>
class Partition : public Iterator<Partition<I>> {
 public:
  Partition(std::vector<size_t> partition_sizes, Iterator<I> &&underlying)
      : m_partition_index(0), m_partition_sizes(std::move(partition_sizes)),
        m_underlying(static_cast<I &&>(underlying)) {}

  Partition(const Partition &) = delete;
  Partition(Partition &&) noexcept = default;
  Partition &operator=(Partition &&) noexcept = default;
  Partition &operator=(const Partition &) = delete;

  [[nodiscard]] std::optional<OutputType<Partition<I>>> next() {
    size_t n = m_partition_index == m_partition_sizes.size()
                     ? std::numeric_limits<size_t>::max()
                     : m_partition_sizes[m_partition_index++];

    auto it = TakeRef<I>(n, m_underlying);
    auto first = it.next();
    if (first.has_value()) {
      return Concat(ArrayMove(std::array{std::move(first.value())}),
                    std::move(it));
    }

    return {};
  }

 private:
  size_t m_partition_index;
  std::vector<size_t> m_partition_sizes;
  I m_underlying;
};

template<typename I>
struct Types<Partition<I>> {
  using Output = Concat<ArrayMove<OutputType<I>, 1>, TakeRef<I>>;
};

template<typename E, typename I>
class PartitionMap : public Iterator<PartitionMap<E, I>> {
 public:
  explicit PartitionMap(std::vector<size_t> partition_sizes, E expr, Iterator<I> &&iter)
      : m_underlying(static_cast<I &&>(iter)),
        m_partition_sizes(std::move(partition_sizes)), m_partition_index(0),
        expr(std::move(expr)) {}

  PartitionMap(const PartitionMap &) = delete;
  PartitionMap(PartitionMap &&) noexcept = default;
  PartitionMap &operator=(PartitionMap &&) noexcept = default;
  PartitionMap &operator=(const PartitionMap &) = delete;

  [[nodiscard]] std::optional<OutputType<PartitionMap<E, I>>> next() {
    size_t n = m_partition_index == m_partition_sizes.size()
               ? std::numeric_limits<size_t>::max()
               : m_partition_sizes[m_partition_index++];

    auto it = TakeRef<I>(n, m_underlying);
    auto first = it.next();
    if (first.has_value()) {
      return expr.apply(Concat(ArrayMove(std::array{std::move(first.value())}),
                               std::move(it)));
    }

    return {};
  }

 private:
  I m_underlying;
  std::vector<size_t> m_partition_sizes;
  size_t m_partition_index;
  E expr;
};

template<typename E, typename I>
struct Types<PartitionMap<E, I>> {
  using Output = expression::OutputType<
  E, Concat<ArrayMove<OutputType<I>, 1>, TakeRef<I>>>;
};

template<typename I1, typename I2>
class Zip : public Iterator<Zip<I1, I2>> {
 public:
  explicit Zip(Iterator<I1> &&left, Iterator<I2> &&right)
      : left(static_cast<I1 &&>(left)), right(static_cast<I2 &&>(right)) {}

  Zip(const Zip &) = delete;
  Zip(Zip &&) noexcept = default;
  Zip &operator=(Zip &&) noexcept = default;
  Zip &operator=(const Zip &) = delete;

  [[nodiscard]] std::optional<OutputType<Zip<I1, I2>>> next() {
    auto l = left.next();
    auto r = right.next();

    if (l.has_value() && r.has_value()) {
      return std::make_pair(std::move(l.value()), std::move(r.value()));
    }

    return {};
  }

 private:
  I1 left;
  I2 right;
};

template<typename I1, typename I2>
struct Types<Zip<I1, I2>> {
  using Output = std::pair<OutputType<I1>, OutputType<I2>>;
};

}// namespace colex::iterator
