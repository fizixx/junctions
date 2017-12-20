
#ifndef JUNCTIONS_ENTITY_H_
#define JUNCTIONS_ENTITY_H_

#include <array>
#include <bitset>
#include <limits>
#include <memory>

#include "junctions/Utils.h"
#include "nucleus/Containers/BitSet.h"
#include "nucleus/Containers/DynamicArray.h"
#include "nucleus/Logging.h"
#include "nucleus/Macros.h"
#include "nucleus/Memory/ScopedPtr.h"
#include "nucleus/Types.h"
#include "nucleus/Utils/Move.h"

namespace ju {

using ComponentId = USize;

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

struct ComponentWrapperBase {
  virtual ~ComponentWrapperBase() {}
};

template <typename ComponentType>
struct ComponentWrapper : public ComponentWrapperBase {
  nu::ScopedPtr<ComponentType> component;

  ComponentWrapper(nu::ScopedPtr<ComponentType> component) : component(move(component)) {}
  virtual ~ComponentWrapper() {}
};

}  // namespace detail

class Entity;
class EntityManager;

using EntityId = nu::DynamicArray<Entity>::SizeType;

static constexpr EntityId kInvalidEntityId = std::numeric_limits<EntityId>::max();

class Entity {
public:
  static constexpr USize kMaxComponents = 16;

  // using ComponentMask = std::bitset<kMaxComponents>;
  using ComponentMask = nu::BitSet<kMaxComponents>;

  explicit Entity(EntityId entityId) : m_id(entityId), m_remove(false) {}

  Entity(Entity&& other) {
    using std::swap;
    swap(m_id, other.m_id);
    swap(m_mask, other.m_mask);
    swap(m_components, other.m_components);
  }

  // Returns this entity's ID.
  ComponentId getId() const {
    return m_id;
  }

  // Mark this entity for removal.
  void remove() {
    m_remove = true;
  }

  // Returns true if this entity has the specified component.
  template <typename... ComponentTypes>
  bool hasComponents() {
    ComponentMask mask = createMask<ComponentTypes...>();

    // Return whether the mask has the bits set or not.
    return (m_mask & mask) == mask;
  }

  // Returns true if our component mask contains those given.
  bool hasComponents(const ComponentMask& mask) {
    return (m_mask & mask) == mask;
  }

  // Returns the component mask for this entity.
  const ComponentMask& getMask() const {
    return m_mask;
  }

  // Add a component to this entity and return the newly created component.
  template <typename ComponentType, typename... Args>
  ComponentType* addComponent(Args&&... args) {
    using WrapperType = detail::ComponentWrapper<ComponentType>;

    // Get the ID for the component.
    ComponentId componentId = detail::getComponentId<ComponentType>();

    ComponentType* component = new ComponentType(nu::forward<Args>(args)...);

    // Add the new component to our list of components.
    m_components[componentId].reset(new WrapperType{component});

    // Set the component in our mask.
    m_mask.set(componentId);

    // Return the new component.
    return component;
  }

  // Get the specified component from this entity.  Returns null if this entity
  // doesn't have the specified type of component.
  template <typename ComponentType>
  ComponentType* getComponent() const {
    using WrapperType = detail::ComponentWrapper<ComponentType>;

    // Get the ID for the component.
    ComponentId componentId = detail::getComponentId<ComponentType>();

    // Get the wrapper.
    const auto& wrapper = m_components.at(componentId);
    if (!wrapper) {
      return nullptr;
    }

    // Return the correct type from the wrapper.
    return static_cast<WrapperType*>(wrapper.get())->component.get();
  }

  bool operator==(const Entity& right) const {
    return m_id == right.m_id;
  }

  bool operator==(EntityId otherId) const {
    return m_id == otherId;
  }

  bool operator!=(const Entity& right) const {
    return !operator==(right);
  }

  bool operator!=(EntityId otherId) const {
    return !operator==(otherId);
  }

private:
  friend class EntityManager;

  template <typename ComponentType>
  static Entity::ComponentMask createMask() {
    Entity::ComponentMask mask;
    mask.set(detail::getComponentId<ComponentType>());
    return mask;
  }

  template <typename C1, typename C2, typename... ComponentTypes>
  static Entity::ComponentMask createMask() {
    return createMask<C1>() | createMask<C2, ComponentTypes...>();
  }

  // Reset the entity to a blank state.
  void resetInternal();

  // The ID of the component.  This is unique per entity manager and won't change after the entity was created.
  EntityId m_id;

  // We build up a mask with each bit representing a component that we have.
  ComponentMask m_mask;

  // Map component type id's to component instances.
  std::array<nu::ScopedPtr<detail::ComponentWrapperBase>, kMaxComponents> m_components;

  // Set to true if the entity should be removed on next update.
  bool m_remove;

  DISALLOW_COPY_AND_ASSIGN(Entity);
};

}  // namespace ju

#endif  // JUNCTIONS_ENTITY_H_
