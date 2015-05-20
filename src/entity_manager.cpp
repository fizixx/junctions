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

#include "junctions/entity_manager.h"

namespace ju {

#if 0
EntityManager::EntitiesView::EntitiesView(EntityManager* entityManager,
                                          size_t count,
                                          const ComponentMask& mask)
  : m_entityManager(entityManager), m_count(count), m_mask(mask) {}
#endif  // 0

Entity EntityManager::createEntity() {
  // Get the next id we can allocate to an entity.
  size_t nextId = m_nextEntityIndex++;

  // Make sure we have space for the new entity.
  ensureEntityCount(nextId + 1);

  // Return the new entity.
  return Entity{this, nextId};
}

void EntityManager::ensureEntityCount(size_t count) {
  m_componentMasks.resize(count);
  for (auto& pool : m_componentPools) {
    if (pool) {
      pool->ensureSize(count);
    }
  }
}

}  // namespace ju
