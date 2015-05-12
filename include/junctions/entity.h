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

#include "junctions/utils.h"

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

class Entity {
public:
  static const size_t kMaxComponent = 64;

  Entity();
  Entity(Entity&& other);

  // Add a component to this entity.
  template <typename ComponentType, typename... Args>
  void addComponent(Args&&... args) {
    // Get the ID for the component.
    ComponentId componentId = detail::getComponentId<ComponentType>();

    // Add the new component to our list of components.
    m_components[componentId] = new ComponentType(std::forward<Args>(args)...);

    // Set the component in our flags.
    m_componentFlags.set(componentId);
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

  // Returns true if this entity has the specified component.
  template <typename ComponentType>
  bool hasComponent() {
    // Get the ID of the component.
    ComponentId componentId = detail::getComponentId<ComponentType>();

    // Return whether the flags have that bit set or not.
    return m_componentFlags.test(componentId);
  }

private:
  // We set a bit for each component we add to the entity.
  std::bitset<kMaxComponent> m_componentFlags;

  // Map component type id's to component instances.
  std::array<void*, kMaxComponent> m_components;

  DISALLOW_COPY_AND_ASSIGN(Entity);
};

}  // namespace ju

#endif  // JUNCTIONS_ENTITY_H_
