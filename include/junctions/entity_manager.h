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

#include <nucleus/logging.h>
#include <nucleus/macros.h>

#include "junctions/entity.h"

namespace ju {

class EventManager;

class EntityManager {
public:
  // Iterator we use to traverse all the entities in the manager.
  class Iterator : public std::iterator<std::input_iterator_tag, Entity*> {
  public:
    // Construct an Iterator with the specified manager and index.
    Iterator(EntityManager* manager, size_t index,
             const Entity::ComponentMask& mask)
      : m_manager(manager), m_index(index), m_mask(mask) {
      next();
    }

    Iterator& operator++() {
      ++m_index;
      next();
      return *this;
    }

    bool operator==(const Iterator& other) const {
      return m_index == other.m_index;
    }
    bool operator!=(const Iterator& other) const {
      return m_index != other.m_index;
    }
    Entity& operator*() { return m_manager->m_entities[m_index]; }
    const Entity& operator*() const { return m_manager->m_entities[m_index]; }

  private:
    // Move the index to the next entity that matches our mask.
    void next() {
      while (!m_manager->m_entities[m_index].hasComponents(m_mask) &&
             m_index != m_manager->m_entities.size()) {
        ++m_index;
      }
    }

    // The manager we are iterating over.
    EntityManager* m_manager;

    // The current index into the list of entiries.
    size_t m_index;

    // The mask we are filtering on.
    Entity::ComponentMask m_mask;
  };

  class EntitiesView {
  public:
    EntitiesView(EntityManager* entityManager, size_t count,
                 const Entity::ComponentMask& mask);
    ~EntitiesView() = default;

    Iterator begin() { return Iterator(m_entityManager, 0, m_mask); }
    Iterator end() { return Iterator(m_entityManager, m_count, m_mask); }

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

private:
  friend class Iterator;

  // The event manager we use to send events.
  EventManager* m_eventManager;

  // All the entities that we own.
  std::vector<Entity> m_entities;

  DISALLOW_IMPLICIT_CONSTRUCTORS(EntityManager);
};

}  // namespace ju

#endif  // JUNCTIONS_ENTITY_MANAGER_H_
