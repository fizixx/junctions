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

#include <memory>
#include <unordered_map>

#include <nucleus/macros.h>

#include "junctions/utils.h"

namespace ju {

class Entity {
public:
  Entity() = default;

  Entity(Entity&& other);

  // Add a component to this entity.
  template <typename ComponentType, typename... Args>
  void addComponent(Args&&... args) {
    // Get the ID for the component.
    size_t componentId = IdForType<ComponentType>::getId();

    // Create the component.
    auto newComponent = new ComponentType(std::forward<Args>(args)...);

    // Add the component to our list of components.
    m_components.insert(std::make_pair(componentId, newComponent));
  }

  // Get the specified component from this entity.  Returns null if this entity
  // doesn't have the specified type of component.
  template <typename ComponentType>
  ComponentType* getComponent() {
    // Get the ID for the component.
    size_t componentId = IdForType<ComponentType>::getId();

    // Find the component and return it if we have it.
    auto it = m_components.find(componentId);
    if (it != std::end(m_components)) {
      return static_cast<ComponentType*>(it->second);
    }

    // This entity doesn't have the specified component.
    return nullptr;
  }

private:
  // Map component type id's to component instances.
  std::unordered_map<size_t, void*> m_components;

  DISALLOW_COPY_AND_ASSIGN(Entity);
};

}  // namespace ju

#endif  // JUNCTIONS_ENTITY_H_
