
#include "junctions/system_manager.h"

#include "nucleus/MemoryDebug.h"

namespace ju {

SystemManager::SystemManager(EntityManager* entityManager) : m_entityManager(entityManager) {}

SystemManager::~SystemManager() {}

}  // namespace ju
