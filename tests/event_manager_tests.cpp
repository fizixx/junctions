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

#include <gtest/gtest.h>

#include <nucleus/logging.h>
#include "junctions/event_manager.h"

namespace ju {

struct SomeEvent {
  int count{0};

  SomeEvent(int count) : count(count) {}
};

struct AnotherEvent {
  std::string word;
  AnotherEvent(const std::string& word) : word(word) {}
};

class IReceiveEvents {
public:
  IReceiveEvents() = default;

  void receive(const SomeEvent& event) {
    LOG(Info) << event.count;
  }

  void receive(const AnotherEvent& event) {
    LOG(Info) << event.word;
  }
};

TEST(EventManagerTest, Basic) {
  EventManager em;

  IReceiveEvents ire;

  em.subscribe<SomeEvent>(&ire);
  em.subscribe<SomeEvent>(&ire);
  em.subscribe<AnotherEvent>(&ire);

  em.emit<SomeEvent>(10);
  em.emit<AnotherEvent>("Another event");
}

}  // namespace ju
