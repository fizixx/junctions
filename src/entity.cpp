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

#include "junctions/entity.h"

namespace ju {

Entity::Entity(EntityId entityId) : m_id(entityId) {}

Entity::Entity(Entity&& other) {
    using std::swap;
    swap(m_id, other.m_id);
    swap(m_mask, other.m_mask);
    swap(m_components, other.m_components);
}

void Entity::remove() {
    m_remove = true;
}

void Entity::resetInternal() {
    // Reset the ID to an invalid ID.
    m_id = kInvalidEntityId;

    // Delete all the components.
    for (auto& component : m_components) {
        component.reset();
    }

    // Reset the component mask.
    m_mask = 0;

    // Don't remove the next entity.
    m_remove = false;
}

}  // namespace ju
