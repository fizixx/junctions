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

#ifndef JUNCTIONS_SYSTEM_MANAGER_H_
#define JUNCTIONS_SYSTEM_MANAGER_H_

#include <unordered_map>

#include "nucleus/logging.h"
#include "nucleus/macros.h"

#include "junctions/utils.h"

namespace ju {

class EntityManager;
class EventManager;

namespace detail {

template <typename SystemType>
struct SystemDeleter {
  static void deleteSystem(void* system) {
    delete static_cast<SystemType*>(system);
  }
};

template <typename SystemType>
struct HasConfigureFunction {
  template <typename C>
  static std::true_type check(C*, decltype(SystemType::configure)* = 0);
  static std::false_type check(...);

  enum { value = decltype(check(static_cast<SystemType*>(0)))::value };
};

template <typename SystemType>
void callConfigure(
    typename std::enable_if<!HasConfigureFunction<SystemType>::value,
                            SystemType>::type* system,
    EventManager* eventManager) {
  LOG(Warning) << "System not configurable!";
}

template <typename SystemType>
void callConfigure(
    typename std::enable_if<HasConfigureFunction<SystemType>::value,
                            SystemType>::type* system,
    EventManager* eventManager) {
  DCHECK(eventManager);
  system->configure(*eventManager);
}

}  // namespace detail

class SystemManager {
public:
  SystemManager(EntityManager* entityManager, EventManager* eventManager);
  ~SystemManager();

  // Add a system to the system manager.
  template <typename SystemType, typename... Args>
  void addSystem(Args&&... args) {
    // Get the id of the system.
    size_t systemId = IdForType<SystemType>::getId();

    // We can't add a duplicate system.
    DCHECK(m_systems.find(systemId) == std::end(m_systems));

    // Create the new system.
    SystemType* system = new SystemType{std::forward<Args>(args)...};

    // Configure the new system.
    detail::callConfigure<SystemType>(system, m_eventManager);

    // Create the details for the system.
    SystemDetails details{system,
                          &detail::SystemDeleter<SystemType>::deleteSystem};

    // Insert the new system into our map.
    m_systems.insert(std::make_pair(systemId, details));
  }

  // Get the instance of the system from the manager.  Returns null if the
  // system doesn't exist in the manager.
  template <typename SystemType>
  SystemType* getSystem() {
    // Get the id for the system.
    size_t systemId = IdForType<SystemType>::getId();

    // Get the system.
    auto it = m_systems.find(systemId);
    if (it == std::end(m_systems)) {
      return nullptr;
    }

    return static_cast<SystemType*>(it->second.system);
  }

  // Update the specified system with the specified adjustment.  Returns true if
  // the system was updated successfully.  Returns false if the system doesn't
  // exist in this manager.
  template <typename SystemType, typename... Args>
  bool update(Args&&... args) {
    // Get the id for the system.
    size_t systemId = IdForType<SystemType>::getId();

    // Get the system.
    auto it = m_systems.find(systemId);
    if (it == std::end(m_systems)) {
      NOTREACHED();
      return false;
    }

    // Get the system from the SystemDetails.
    SystemType* system = static_cast<SystemType*>(it->second.system);

    // Update the system.
    system->update(*m_entityManager, *m_eventManager,
                   std::forward<Args>(args)...);

    // Success.
    return true;
  }

private:
  // The details of the system we're storing.
  struct SystemDetails {
    void* system;
    void (*deleter)(void*);
  };

  // The entity manager we pass to all the systems.
  EntityManager* m_entityManager;

  // The event manager we use to send events.
  EventManager* m_eventManager;

  // The map of systems with their ids.
  std::unordered_map<size_t, SystemDetails> m_systems;

  DISALLOW_IMPLICIT_CONSTRUCTORS(SystemManager);
};

}  // namespace ju

#endif  // JUNCTIONS_SYSTEM_MANAGER_H_
