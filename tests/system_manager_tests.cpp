
#include <gtest/gtest.h>

#include "junctions/event_manager.h"
#include "junctions/system_manager.h"

namespace ju {

struct BlankSystem {};

struct ConfigureSystem {
  bool configureCalled{false};

  void configure(EventManager& eventManager) { configureCalled = true; }
};

TEST(SystemManagerTest, blah) {
  EXPECT_FALSE(detail::HasConfigureFunction<BlankSystem>::value);
  EXPECT_TRUE(detail::HasConfigureFunction<ConfigureSystem>::value);
}

TEST(SystemManagerTest, AddSystem) {
  EventManager evtMan;
  SystemManager sm{nullptr, &evtMan};

  sm.addSystem<BlankSystem>();
  sm.addSystem<ConfigureSystem>();
  EXPECT_TRUE(sm.getSystem<ConfigureSystem>()->configureCalled);
}

}  // namespace ju
