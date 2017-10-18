
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

  int total{0};
  std::string finalString;

  void receive(const SomeEvent& event) { total += event.count; }

  void receive(const AnotherEvent& event) { finalString.append(event.word); }
};

TEST(EventManagerTest, Basic) {
  EventManager em;

  IReceiveEvents ire;

  em.subscribe<SomeEvent>(&ire);
  em.subscribe<AnotherEvent>(&ire);

  em.emit<SomeEvent>(10);
  em.emit<AnotherEvent>("event");
  em.emit<SomeEvent>(20);
  em.emit<AnotherEvent>("Another");

  EXPECT_EQ(30, ire.total);
  EXPECT_EQ(std::string{"eventAnother"}, ire.finalString);
}

}  // namespace ju
