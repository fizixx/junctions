
#include "junctions/Entity.h"

#include "nucleus/MemoryDebug.h"

namespace ju {

void Entity::resetInternal() {
  // Reset the ID to an invalid ID.
  m_id = kInvalidEntityId;

  // Delete all the components.
  for (auto& component : m_components) {
    component.reset();
  }

  // Reset the component mask.
  m_mask.reset();

  // Don't remove the next entity.
  m_remove = false;
}

}  // namespace ju
