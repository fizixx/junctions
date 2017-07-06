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

#include "nucleus/macros.h"
#include "nucleus/logging.h"

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
    // Get the ID for the component.
    ComponentId componentId = detail::getComponentId<ComponentType>();

    // Add the new component to our list of components.
    m_components[componentId] = new ComponentType(std::forward<Args>(args)...);

    // Set the component in our mask.
    m_mask.set(componentId);
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
  // We build up a mask with each bit representing a component that we have.
  ComponentMask m_mask;

  // Map component type id's to component instances.
  std::array<void*, kMaxComponents> m_components;

  DISALLOW_COPY_AND_ASSIGN(Entity);
};

}  // namespace ju

#endif  // JUNCTIONS_ENTITY_H_
