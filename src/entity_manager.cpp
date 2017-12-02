
#include "junctions/entity_manager.h"

#include "nucleus/Memory/ScopedPtr.h"

#include "nucleus/MemoryDebug.h"

namespace ju {

EntityManager::EntitiesView::EntitiesView(EntityManager* entityManager, size_t count, const Entity::ComponentMask& mask)
  : m_entityManager(entityManager), m_count(count), m_mask(mask) {}

EntityId EntityManager::createEntity() {
  auto nextEntityId = m_entities.getSize();
  m_entities.emplaceBack(nu::MakeScopedPtr<Entity>(nextEntityId));
  return nextEntityId;
}

Entity* EntityManager::getEntity(EntityId id) {
  return m_entities.get(id).get();
}

void EntityManager::update() {
  cleanUpEntities();
}

void EntityManager::cleanUpEntities() {
  // Remove all entities that are marked for removal.
  for (EntitiesType::SizeType i = 0; i < m_entities.getSize(); ++i) {
    auto& entity = m_entities.get(i);
    if (entity && entity->m_remove) {
      m_entities.get(i).reset();
    }
  }
}

}  // namespace ju
