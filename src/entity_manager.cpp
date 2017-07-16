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

#include "nucleus/Memory/ScopedPtr.h"

namespace ju {

EntityManager::EntitiesView::EntitiesView(EntityManager* entityManager, size_t count, const Entity::ComponentMask& mask)
  : m_entityManager(entityManager), m_count(count), m_mask(mask) {}

EntityId EntityManager::createEntity() {
    auto nextEntityId = m_entities.size();
    m_entities.emplace_back(nu::MakeScopedPtr<Entity>(nextEntityId));
    return nextEntityId;
}

Entity* EntityManager::getEntity(EntityId id) {
    return m_entities[id].get();
}

void EntityManager::update() {
    cleanUpEntities();
}

void EntityManager::cleanUpEntities() {
    // Remove all entities that are marked for removal.
    for (std::vector<Entity>::size_type i = 0; i < m_entities.size(); ++i) {
        auto& entity = m_entities[i];
        if (entity && entity->m_remove) {
            m_entities[i].reset();
        }
    }
}

}  // namespace ju
