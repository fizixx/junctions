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

#ifndef JUNCTIONS_EVENT_MANAGER_H_
#define JUNCTIONS_EVENT_MANAGER_H_

#include <functional>
#include <memory>
#include <unordered_map>

#include <nucleus/macros.h>
#include <nucleus/utils/signals.h>

namespace ju {

namespace detail {

template <typename EventType>
// EventType: The type of the event we want an ID for.
struct EventLookup {
  static size_t getId() { return reinterpret_cast<size_t>(&getId); }
};

template <typename EventType>
// EventType: The type of the event we are wrapping up.
struct SignalWrapper {
  using CallbackType = std::function<void(const EventType&)>;

  CallbackType callback;

  SignalWrapper(const CallbackType& callback) : callback(callback) {}

  void operator()(const void* payload) {
    callback(*static_cast<const EventType*>(payload));
  }
};

}  // namespace detail

class EventManager {
public:
  EventManager() = default;
  ~EventManager() = default;

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
    using Signature = void (ReceiverType::*)(const EventType&);
    Signature receiveFunc = &ReceiverType::receive;

    // Get the signal for this event type.
    SignalType* signal = getSignalFor<EventType>();
    DCHECK(signal);

    // Connect a signal wrapper to the signal.
    signal->connect(detail::SignalWrapper<EventType>{
        std::bind(receiveFunc, receiver, std::placeholders::_1)});
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
    signal->emit(&event);
  }

private:
  using SignalType = nu::Signal<void(const void*)>;

  template <typename EventType>
  // EventType: The type of the event we want the signal for.
  SignalType* getSignalFor() {
    size_t eventId = detail::EventLookup<EventType>::getId();

    // Find the signal in the map.
    auto it = m_signals.find(eventId);

    // If the signal doesn't exist, create it.
    if (it == std::end(m_signals)) {
      auto result = m_signals.insert(
          std::make_pair(eventId, std::make_unique<SignalType>()));
      DCHECK(result.second) << "The signal must have been inserted.";
      it = result.first;
    }

    return it->second.get();
  }

  std::unordered_map<size_t, std::unique_ptr<SignalType>> m_signals;

  DISALLOW_COPY_AND_ASSIGN(EventManager);
};

}  // namespace ju

#endif  // JUNCTIONS_EVENT_MANAGER_H_
