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

class Entity {
public:
  using Id = size_t;

  static const size_t kMaxComponents = 16;

  using ComponentMask = std::bitset<kMaxComponents>;

  Entity();
  Entity(Entity&& other);

  // Returns true if this entity has the specified component.
  template <typename ComponentType>
  bool hasComponent() {
    // Get the ID of the component.
    ComponentId componentId = detail::getComponentId<ComponentType>();

    // Return whether the mask has that bit set or not.
    return m_mask.test(componentId);
  }

  // Returns the component mask for this entity.
  const ComponentMask& getMask() const { return m_mask; }

  // Returns true if our component mask contains those given.
  bool hasComponents(const ComponentMask& mask) {
    return (m_mask & mask) == mask;
  }

  // Add a component to this entity.
  template <typename ComponentType, typename... Args>
  void addComponent(Args&&... args) {
    // Add a component through the manager.
    m_entityManager->addComponent<ComponentType>(m_id,
                                                 std::forward<Args>(args)...);
  }

  // Get the specified component from this entity.  Returns null if this entity
  // doesn't have the specified type of component.
  template <typename ComponentType>
  ComponentType* getComponent() {
    // Get the ID for the component.
    ComponentId componentId = detail::getComponentId<ComponentType>();

    // Find the component and return it if we have it.
    return static_cast<ComponentType*>(m_components[componentId]);
  }

private:
  friend class EntityManager;

  // The EntityManager that created us.
  EntityManager* m_entityManager;

  // The Id we were assigned by the EntityManager when we were created.
  Id m_id;

  // We build up a mask with each bit representing a component that we have.
  ComponentMask m_mask;

  // Map component type id's to component instances.
  std::array<void*, kMaxComponents> m_components;
};

}  // namespace ju

#endif  // JUNCTIONS_ENTITY_H_
