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

class EntityManager {
public:
  // Iterator we use to traverse all the entities in the manager.
  class Iterator : public std::iterator<std::input_iterator_tag, Entity*> {
  public:
    // Construct an Iterator with the specified manager and index.
    Iterator(EntityManager* manager, size_t index,
             const Entity::ComponentFlags& flags)
      : m_manager(manager), m_index(index), m_flags(flags) {
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
    // Move the index to the next entity that matches our flags.
    void next() {
      while (!m_manager->m_entities[m_index].containsComponents(m_flags) &&
             m_index != m_manager->m_entities.size()) {
        ++m_index;
      }
    }

    // The manager we are iterating over.
    EntityManager* m_manager;

    // The current index into the list of entiries.
    size_t m_index;

    // The flags we are filtering.
    Entity::ComponentFlags m_flags;
  };

  class EntitiesView {
  public:
    EntitiesView(EntityManager* entityManager, size_t count,
                 const Entity::ComponentFlags& flags);
    ~EntitiesView() = default;

    Iterator begin() { return Iterator(m_entityManager, 0, m_flags); }
    Iterator end() { return Iterator(m_entityManager, m_count, m_flags); }

  private:
    // The entity manager we are iterating over.
    EntityManager* m_entityManager;

    // The maximum number of components in the list.
    size_t m_count;

    // The flag set we are checking for.
    Entity::ComponentFlags m_flags;
  };

  EntityManager() = default;
  ~EntityManager() = default;

  // Add a new entity to this manager and return the newly created entity.
  Entity* createEntity();

  template <typename ComponentType>
  Entity::ComponentFlags createFlags() {
    Entity::ComponentFlags flags;
    flags.set(detail::getComponentId<ComponentType>());
    return flags;
  }

  template <typename C1, typename C2, typename... ComponentTypes>
  Entity::ComponentFlags createFlags() {
    return createFlags<C1>() | createFlags<C2, ComponentTypes...>();
  }

  // Return a view of all entities in the manager.
  template <typename... ComponentTypes>
  EntitiesView allEntitiesWithComponent() {
    Entity::ComponentFlags flags = createFlags<ComponentTypes...>();
    return EntitiesView{this, m_entities.size(), flags};
  }

private:
  friend class Iterator;

  // All the entities that we own.
  std::vector<Entity> m_entities;

  DISALLOW_COPY_AND_ASSIGN(EntityManager);
};

}  // namespace ju

#endif  // JUNCTIONS_ENTITY_MANAGER_H_
