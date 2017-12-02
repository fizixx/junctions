
#include "junctions/entity.h"

#include "nucleus/MemoryDebug.h"

namespace ju {

Entity::Entity(EntityId entityId) : m_id(entityId) {}

Entity::Entity(Entity&& other) {
  using std::swap;
  swap(m_id, other.m_id);
  swap(m_mask, other.m_mask);
  swap(m_components, other.m_components);
}

void Entity::remove() {
  m_remove = true;
}

void Entity::resetInternal() {
  // Reset the ID to an invalid ID.
  m_id = kInvalidEntityId;

  // Delete all the components.
  for (auto& component : m_components) {
    component.reset();
  }

  // Reset the component mask.
  m_mask = 0;

  // Don't remove the next entity.
  m_remove = false;
}

}  // namespace ju
