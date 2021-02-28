#pragma once

#include "../inc/interface.hpp"

namespace colex::iterator {

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


}

