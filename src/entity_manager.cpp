
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
