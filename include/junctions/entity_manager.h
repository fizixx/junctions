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
    Iterator(EntityManager* manager, size_t index = 0)
      : m_manager(manager), m_index(index) {}

    Iterator& operator++() {
      ++m_index;
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
    // The manager we are iterating over.
    EntityManager* m_manager;

    // The current index into the list of entiries.
    size_t m_index;
  };

  EntityManager() = default;

  // Add a new entity to this manager and return the newly created entity.
  Entity* createEntity();

  template <typename... ComponentTypes>
  bool unpack(Entity& entity) const {
    return unpack_<0, ComponentTypes...>(entity);
  }

  template <size_t N, typename ComponentType>
  bool unpack_(Entity& entity) const {
    LOG(Info) << "0: " << IdForType<ComponentType>::getId();
    return true;
  }

  template <size_t N, typename ComponentType1, typename ComponentType2, typename... ComponentTypes>
  bool unpack_(Entity& entity) const {
    LOG(Info) << "1: " << IdForType<ComponentType1>::getId();
    return unpack_<N + 1, ComponentType2, ComponentTypes...>(entity);
  }

  // Return a view of all entities in the manager.
  template <typename... ComponentTypes>
  void allEntitiesWithComponent() {
    for (auto& entity : m_entities) {
      bool hasComponent = unpack<ComponentTypes...>(entity);
      LOG(Info) << hasComponent;
    }
  }
  

  // Iteration
  Iterator begin() { return Iterator(this); }
  Iterator end() { return Iterator(this, m_entities.size()); }

private:
  // All the entities that we own.
  std::vector<Entity> m_entities;

  DISALLOW_COPY_AND_ASSIGN(EntityManager);
};

}  // namespace ju

#endif  // JUNCTIONS_ENTITY_MANAGER_H_
