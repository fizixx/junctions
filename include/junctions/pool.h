// Copyright (c) 2015, Tiaan Louw
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#ifndef JUNCTIONS_POOL_H_
#define JUNCTIONS_POOL_H_

#include <cstdint>
#include <vector>

#include "nucleus/logging.h"
#include "nucleus/macros.h"
#include "nucleus/types.h"

namespace ju {

namespace detail {

// A Pool of data that holds a number of untyped elements.  We can specify the
// element size and the chunk size.  New chunks are added to a list of chunks,
// so existing pointers to elements will never be invalidated.  Data is also
// contiguous per chunk, which makes it cache friendly.
class PoolBase {
public:
  PoolBase(usize elementSize, usize elementsPerChunk)
    : m_elementSize(elementSize), m_elementsPerChunk(elementsPerChunk) {}

  virtual ~PoolBase() {
    // Delete all the chunks we allocated.
    for (auto& chunk : m_chunks) {
      delete[] chunk;
    }
  }

  // Make sure we can hold at least the specified amount of elements in the
  // pool.
  inline void ensureSize(usize size) {
    if (size > m_size) {
      if (size > m_capacity) {
        reserve(size);
      }
      m_size = size;
    }
  }

  // Reserve the specified amount of space.
  inline void reserve(usize size) {
    while (m_capacity < size) {
      m_chunks.emplace_back(new uint8_t[m_elementSize * m_elementsPerChunk]);
      m_capacity += m_elementsPerChunk;
    }
  }

protected:
  // Get a pointer to the element with the specified index.
  inline void* getInternal(usize index) {
    DCHECK(index < m_size);
    return m_chunks[index / m_elementsPerChunk] +
           (index % m_elementsPerChunk) * m_elementSize;
  }

  inline const void* getInternal(usize index) const {
    DCHECK(index < m_size);
    return m_chunks[index / m_elementsPerChunk] +
           (index % m_elementsPerChunk) * m_elementSize;
  }

  // We need a special destroy, so that we can call destructors if needed.
  virtual void destroy(usize index) = 0;

  // A vector to hold a list of all our chunks.
  std::vector<uint8_t*> m_chunks;

  // The size of each element we allocate.
  usize m_elementSize;

  // The amount of elements we hold per chunk.
  usize m_elementsPerChunk;

  // The number of elements we currently have.
  usize m_size{0};

  // The total capacity of elements we can hold.
  usize m_capacity{0};

  DISALLOW_IMPLICIT_CONSTRUCTORS(PoolBase);
};

}  // namespace detail

template <typename T, usize ElementsPerChunk = 64>
class Pool : public detail::PoolBase {
public:
  Pool() : PoolBase(sizeof(T), ElementsPerChunk) {}

  ~Pool() override {}

  // Create a new entity in place at the given index and return the pointer to
  // the newly created element.
  template <typename... Args>
  T* create(usize index, Args&&... args) {
    // Get a pointer to where we should create the new element.
    T* ptr = get(index);

    // Call the constructor of the element.
    new (ptr) T(std::forward<Args>(args)...);

    // Return the element.
    return static_cast<T*>(ptr);
  }

  // Return the item at the specified index.
  T* get(usize index) { return static_cast<T*>(getInternal(index)); }

  // Return the item at the specified index.
  const T* get(usize index) const {
    return static_cast<const T*>(getInternal(index));
  }

  void destroy(usize index) override {
    DCHECK(index < m_size);
    T* ptr = static_cast<T*>(get(index));
    ptr->~T();
  }
};

}  // namespace ju

#endif  // JUNCTIONS_POOL_H_
