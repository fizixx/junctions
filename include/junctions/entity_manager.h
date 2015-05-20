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

#ifndef JUNCTIONS_ENTITY_MANAGER_H_
#define JUNCTIONS_ENTITY_MANAGER_H_

#include <iterator>
#include <set>
#include <vector>

#include "nucleus/logging.h"
#include "nucleus/macros.h"

#include "junctions/entity.h"
#include "junctions/pool.h"

namespace ju {

using ComponentId = std::size_t;

namespace detail {

inline ComponentId getUniqueComponentId() {
  static ComponentId nextId = 0;
  return nextId++;
}

template <typename ComponentType>
inline ComponentId getComponentId() {
  static ComponentId componentId = getUniqueComponentId();
  return componentId;
}

}  // namespace detail

class EventManager;

class EntityManager {
public:
  // Iterator we use to traverse all the entities in the manager.
  class Iterator : public std::iterator<std::input_iterator_tag, Entity> {
  public:
    // Construct an Iterator with the specified manager and index.
    Iterator(EntityManager* manager, size_t index,
             const Entity::ComponentMask& mask)
      : m_manager(manager), m_index(index),
        m_maxSize(m_manager->m_entities.size()), m_mask(mask) {
      next();
    }

    // Construct an end Iterator.
    Iterator(EntityManager* manager)
      : m_manager(manager), m_index(m_manager->m_entities.size()),
        m_maxSize(m_index) {}

    Iterator& operator++() {
      ++m_index;
      next();
      return *this;
    }

    bool operator==(const Iterator& other) const {
      DCHECK(m_manager == other.m_manager)
          << "Can't compare iterators from different managers.";
      return m_index == other.m_index;
    }
    bool operator!=(const Iterator& other) const {
      DCHECK(m_manager == other.m_manager)
          << "Can't compare iterators from different managers.";
      return m_index != other.m_index;
    }
    Entity& operator*() { return m_manager->m_entities[m_index]; }
    const Entity& operator*() const { return m_manager->m_entities[m_index]; }

  private:
    // Move the index to the next entity that matches our mask.
    void next() {
      while (m_index != m_maxSize) {
        if (m_manager->m_entities[m_index].hasComponents(m_mask)) {
          break;
        }
        ++m_index;
      }
    }

    // The manager we are iterating over.
    EntityManager* m_manager;

    // The current index into the list of entiries.
    size_t m_index;

    // The total capacity of the list we are iterating over.
    size_t m_maxSize;

    // The mask we are filtering on.
    Entity::ComponentMask m_mask;
  };

  class EntitiesView {
  public:
    EntitiesView(EntityManager* entityManager, size_t count,
                 const Entity::ComponentMask& mask);
    ~EntitiesView() = default;

    Iterator begin() { return Iterator(m_entityManager, 0, m_mask); }
    Iterator end() { return Iterator(m_entityManager); }

  private:
    // The entity manager we are iterating over.
    EntityManager* m_entityManager;

    // The maximum number of components in the list.
    size_t m_count;

    // The mask we are filtering entities on.
    Entity::ComponentMask m_mask;
  };

  explicit EntityManager(EventManager* eventManager)
    : m_eventManager(eventManager) {}
  ~EntityManager() = default;

  // Add a new entity to this manager and return the newly created entity.
  Entity* createEntity();

  template <typename ComponentType>
  Entity::ComponentMask createMask() {
    Entity::ComponentMask mask;
    mask.set(detail::getComponentId<ComponentType>());
    return mask;
  }

  template <typename C1, typename C2, typename... ComponentTypes>
  Entity::ComponentMask createMask() {
    return createMask<C1>() | createMask<C2, ComponentTypes...>();
  }

  // Return a view of all entities in the manager.
  template <typename... ComponentTypes>
  EntitiesView allEntitiesWithComponent() {
    Entity::ComponentMask mask = createMask<ComponentTypes...>();
    return EntitiesView{this, m_entities.size(), mask};
  }

  // Add a component with the specified type to the specified entity.
  template <typename ComponentType, typename... Args>
  ComponentType* addComponent(Entity::Id entityId, Args&&... args) {
    // Get the id for the component.
    auto componentId = detail::getComponentId<ComponentType>();

    // TODO: Make sure the entity doesn't have this component already.

    // Create the component in the component pools.
    Pool<ComponentType>* componentPool = getComponentPool<ComponentType>();
    ComponentType* component = componentPool->create<ComponentType>(
        entityId, std::forward<Args>(args)...);

    // TODO: Send an event that a new component was added.

    // Return a pointer to the newly created component.
    return component;
  }

  // Get the component with the specified type from the entity with the
  // specified id.
  template <typename ComponentType>
  ComponentType* getComponent(Entity::Id entityId) {
    // Get the id of the component.
    auto componentId = detail::getComponentId<ComponentType>();

    // Get the component from the pool.
    Pool<ComponentType> componentPool = getComponentPool<ComponentType>();
    ComponentType* component = componentPool.get(id);

    // Return the component.
    return component;
  }

private:
  friend class Iterator;

  // Get or create the component pool for the specified component type.
  template <typename ComponentType>
  Pool<ComponentType>* getComponentPool() {
    // Get the component id.
    auto componentId = detail::getComponentId<ComponentType>();

    // Make sure we have an index for this pool.
    if (m_componentPools.size() < componentId) {
      m_componentPools.resize(componentId + 1, nullptr);
    }

    // Create the pool if it doesn't exist already.
    if (!m_componentPools[componeneId]) {
      Pool<ComponentType>* pool = new Pool<ComponentType>();
      pool->ensureSize(m_entities.size());
      m_componentPools[componentId] = pool;
    }

    // Return the pool.
    return static_cast<Pool<ComponentType>*>(m_componentPools[componentId]);
  }

  // The event manager we use to send events.
  EventManager* m_eventManager;

  // All the entities that we own.
  std::vector<Entity> m_entities;

  // A pool of components for each component type.
  std::vector<detail::PoolBase*> m_componentPools;

  DISALLOW_IMPLICIT_CONSTRUCTORS(EntityManager);
};

}  // namespace ju

#endif  // JUNCTIONS_ENTITY_MANAGER_H_
