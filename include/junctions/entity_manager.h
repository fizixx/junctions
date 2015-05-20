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
  static const size_t kMaxComponents = 16;

  using ComponentMask = std::bitset<kMaxComponents>;

#if 0
  // Iterator we use to traverse all the entities in the manager.
  class Iterator : public std::iterator<std::input_iterator_tag, Entity> {
  public:
    // Construct an Iterator with the specified manager and index.
    Iterator(EntityManager* manager, size_t index,
             const ComponentMask& mask)
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
#if 0
        if (m_manager->m_entities[m_index].hasComponents(m_mask)) {
          break;
        }
#endif  // 0
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
#endif  // 0

#if 0
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
#endif  // 0

  explicit EntityManager(EventManager* eventManager)
    : m_eventManager(eventManager) {}

  ~EntityManager() = default;

  // Add a new entity to this manager and return it.
  Entity createEntity();

#if 0
  template <typename ComponentType>
  ComponentMask createMask() {
    Entity::ComponentMask mask;
    mask.set(detail::getComponentId<ComponentType>());
    return mask;
  }

  template <typename C1, typename C2, typename... ComponentTypes>
  ComponentMask createMask() {
    return createMask<C1>() | createMask<C2, ComponentTypes...>();
  }

  // Return a view of all entities in the manager.
  template <typename... ComponentTypes>
  EntitiesView allEntitiesWithComponent() {
    ComponentMask mask = createMask<ComponentTypes...>();
    return EntitiesView{this, m_entities.size(), mask};
  }
#endif  // 0

  // Add a component with the specified type to the specified entity.
  template <typename ComponentType, typename... Args>
  ComponentType* createComponent(const Entity& entity, Args&&... args) {
    // Make sure we don't have the component already.
    DCHECK(!hasComponent<ComponentType>(entity));

    // Get the pool that the component lives in.
    Pool<ComponentType>* componentPool = getComponentPool<ComponentType>();
    DCHECK(componentPool);

    // Get the component.
    ComponentType* component =
        componentPool->create(entity.getId(), std::forward<Args>(args)...);

    // Update the component mask for the entity.
    m_componentMasks[entity.getId()].set(
        detail::getComponentId<ComponentType>());

    // TODO: Send an event that a new component was added.

    // Return a pointer to the newly created component.
    return component;
  }

  // Get the component with the specified type from the entity with the
  // specified id.
  template <typename ComponentType>
  ComponentType* getComponent(const Entity& entity) {
    // Get pool that the component lives in.
    Pool<ComponentType>* componentPool = getComponentPool<ComponentType>();
    DCHECK(componentPool);

    // Return the component.
    return componentPool->get(entity.getId());
  }

  // Returns true if the specified entity has the specified component.
  template <typename ComponentType>
  bool hasComponent(const Entity& entity) {
    // Get the id of the component type.
    auto componentId = detail::getComponentId<ComponentType>();

    // Return the result of testing for the component id in the entity's
    // component mask.
    return m_componentMasks[entity.getId()].test(componentId);
  }

private:
  friend class Iterator;

  // Ensure that we have room for the the given amount of entities.
  void ensureEntityCount(size_t count);

  // Get or create the component pool for the specified component type.
  template <typename ComponentType>
  Pool<ComponentType>* getComponentPool() {
    // Get the component id.
    auto componentId = detail::getComponentId<ComponentType>();

    // Make sure we have an index for this pool.
    if (m_componentPools.size() < componentId + 1) {
      m_componentPools.resize(componentId + 1, nullptr);
    }

    // Create the pool if it doesn't exist already.
    if (!m_componentPools[componentId]) {
      Pool<ComponentType>* pool = new Pool<ComponentType>();
      pool->ensureSize(m_nextEntityIndex);
      m_componentPools[componentId] = pool;
    }

    // Return the pool.
    return static_cast<Pool<ComponentType>*>(m_componentPools[componentId]);
  }

  // The event manager we use to send events.
  EventManager* m_eventManager;

  // A list of all entity component masks.
  std::vector<ComponentMask> m_componentMasks;

  // A pool of components for each component type.
  std::vector<detail::PoolBase*> m_componentPools;

  // The index of the next entity we can create.
  size_t m_nextEntityIndex{0};

  DISALLOW_IMPLICIT_CONSTRUCTORS(EntityManager);
};

}  // namespace ju

#endif  // JUNCTIONS_ENTITY_MANAGER_H_
