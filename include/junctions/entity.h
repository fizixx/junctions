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

#ifndef JUNCTIONS_ENTITY_H_
#define JUNCTIONS_ENTITY_H_

#include <array>
#include <bitset>
#include <memory>

#include <nucleus/macros.h>
#include <nucleus/logging.h>

#include "junctions/utils.h"

namespace ju {

class EntityManager;

class Entity {
public:
  using Id = usize;

  // Create an invalid blank entity.
  Entity();

  Entity(EntityManager* entityManager, Id id)
    : m_entityManager(entityManager), m_id(id) {}

  // Return the id for this entity.
  const Id& getId() const { return m_id; }

  template <typename ComponentType, typename... Args>
  ComponentType* createComponent(Args&&... args) {
    DCHECK(m_entityManager);
    return m_entityManager->createComponent<ComponentType>(
        *this, std::forward<Args>(args)...);
  }

  template <typename ComponentType>
  ComponentType* getComponent() const {
    DCHECK(m_entityManager);
    return m_entityManager->getComponent<ComponentType>(*this);
  }

  // Returns true if this entity has the specified component.
  template <typename ComponentType>
  bool hasComponent() const {
    DCHECK(m_entityManager);
    return m_entityManager->hasComponent<ComponentType>(*this);
  }

private:
  // The EntityManager that created us.
  EntityManager* m_entityManager;

  // The Id we were assigned by the EntityManager when we were created.
  Id m_id;
};

}  // namespace ju

#endif  // JUNCTIONS_ENTITY_H_
