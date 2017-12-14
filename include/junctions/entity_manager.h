
#ifndef JUNCTIONS_ENTITY_MANAGER_H_
#define JUNCTIONS_ENTITY_MANAGER_H_

#include <iterator>
#include <set>
#include <unordered_map>

#include "junctions/entity.h"
#include "nucleus/Containers/DynamicArray.h"
#include "nucleus/logging.h"
#include "nucleus/macros.h"
#include "nucleus/utils/signals.h"

namespace ju {

class EntityManager;

namespace detail {

template <typename EventType>
// EventType: The type of the event we are wrapping up.
struct SignalWrapper {
  using CallbackType = std::function<void(EntityManager&, const EventType&)>;

  CallbackType callback;

  SignalWrapper(const CallbackType& callback) : callback(callback) {}

  void operator()(EntityManager& entities, const void* payload) {
    callback(entities, *static_cast<const EventType*>(payload));
  }
};

}  // namespace detail

class EntityManager {
public:
  // Iterator we use to traverse all the entities in the manager.
  class Iterator : public std::iterator<std::input_iterator_tag, Entity> {
  public:
    // Construct an Iterator with the specified manager and index.
    Iterator(EntityManager* manager, size_t index, const Entity::ComponentMask& mask)
      : m_manager(manager), m_index(index), m_maxSize(m_manager->m_entities.getSize()), m_mask(mask) {
      next();
    }

    // Construct an end Iterator.
    Iterator(EntityManager* manager)
      : m_manager(manager), m_index(m_manager->m_entities.getSize()), m_maxSize(m_index) {}

    Iterator& operator++() {
      ++m_index;
      next();
      return *this;
    }

    bool operator==(const Iterator& other) const {
      DCHECK(m_manager == other.m_manager) << "Can't compare iterators from different managers.";
      return m_index == other.m_index;
    }
    bool operator!=(const Iterator& other) const {
      DCHECK(m_manager == other.m_manager) << "Can't compare iterators from different managers.";
      return m_index != other.m_index;
    }
    Entity& operator*() { return *m_manager->m_entities.get(m_index); }
    const Entity& operator*() const { return *m_manager->m_entities.get(m_index); }

  private:
    // Move the index to the next entity that matches our mask.
    void next() {
      while (m_index != m_maxSize) {
        auto& entity = m_manager->m_entities.get(m_index);
        if (entity && entity->hasComponents(m_mask)) {
          break;
        }
        ++m_index;
      }
    }

    // The manager we are iterating over.
    EntityManager* m_manager;

    // The current index into the list of entiries.
    size_t m_index;

    // The total capacity of the list we are iterating over.
    size_t m_maxSize;

    // The mask we are filtering on.
    Entity::ComponentMask m_mask;
  };

  class EntitiesView {
  public:
    EntitiesView(EntityManager* entityManager, size_t count, const Entity::ComponentMask& mask);
    ~EntitiesView() = default;

    Iterator begin() { return Iterator(m_entityManager, 0, m_mask); }
    Iterator end() { return Iterator(m_entityManager); }

  private:
    // The entity manager we are iterating over.
    EntityManager* m_entityManager;

    // The maximum number of components in the list.
    size_t m_count;

    // The mask we are filtering entities on.
    Entity::ComponentMask m_mask;
  };

  EntityManager() {}
  ~EntityManager() {}

  // Add a new entity to this manager and return the newly created entity.
  EntityId createEntity();

  // Return a pointer to the entity with the given ID.
  Entity* getEntity(EntityId id);

  // Return the component from the entity with the given ID.
  template <typename ComponentType>
  ComponentType* getComponent(EntityId id) const {
#if BUILD(DEBUG)
    if (id >= m_entities.getSize()) {
      return nullptr;
    }
#endif  // BUILD(DEBUG)

    const Entity& entity = *m_entities.get(id);
    return entity.getComponent<ComponentType>();
  }

  // Return a view of all entities in the manager.
  template <typename... ComponentTypes>
  EntitiesView allEntitiesWithComponent() {
    Entity::ComponentMask mask = Entity::createMask<ComponentTypes...>();
    return EntitiesView{this, m_entities.getSize(), mask};
  }

  void update();

  // Subscribe the specified receiver to events of EventType.  The receiver must
  // have a member function similar to this:
  //
  //   struct Receiver {
  //     void receive(const EventType& event) {
  //       // Do something with event.
  //     }
  //   };
  //
  // Where EventType is the type of event you subscribe for.
  template <typename EventType, typename ReceiverType>
  // EventType: The type of the event we are subscribing for.
  // ReceiverType: The type of the object that will be receiving the event.
  void subscribe(ReceiverType* receiver) {
    // Get the pointer to the receive function that will be called when we emit
    // a signal of this type.
    using Signature = void (ReceiverType::*)(EntityManager&, const EventType&);
    Signature receiveFunc = &ReceiverType::receive;

    // Get the signal for this event type.
    SignalType* signal = getSignalFor<EventType>();
    DCHECK(signal);

    // Connect a signal wrapper to the signal.
    signal->connect(detail::SignalWrapper<EventType>{
        std::bind(receiveFunc, receiver, std::placeholders::_1, std::placeholders::_2)});
  }

  // Emit the given event with the parameters specified.
  template <typename EventType, typename... Args>
  // EventType: The type of the event we want to emit.
  void emit(Args&&... args) {
    // Create the event object we are going to send.
    // EventType event(std::forward<Args>(args)...);
    EventType event = EventType(std::forward<Args>(args)...);
    // We can't use the above line, because MSVC gives internal error.

    // Get the signal we want to emit the event on.
    SignalType* signal = getSignalFor<EventType>();
    DCHECK(signal);

    // Emit the event.
    signal->emit(*this, &event);
  }

private:
  friend class Iterator;

  using SignalType = nu::Signal<void(EntityManager&, const void*)>;

  void cleanUpEntities();

  template <typename EventType>
  // EventType: The type of the event we want the signal for.
  SignalType* getSignalFor() {
    size_t eventId = IdForType<EventType>::getId();

    // Find the signal in the map.
    auto it = m_signals.find(eventId);

    // If the signal doesn't exist, create it.
    if (it == std::end(m_signals)) {
      auto result = m_signals.insert(std::make_pair(eventId, std::make_unique<SignalType>()));
      DCHECK(result.second) << "The signal must have been inserted.";
      it = result.first;
    }

    return it->second.get();
  }

  // All the entities that we own.
  using EntitiesType = nu::DynamicArray<nu::ScopedPtr<Entity>>;
  EntitiesType m_entities;

  // Signals that we use to emit events.
  std::unordered_map<size_t, std::unique_ptr<SignalType>> m_signals;

  DISALLOW_COPY_AND_ASSIGN(EntityManager);
};

}  // namespace ju

#endif  // JUNCTIONS_ENTITY_MANAGER_H_
