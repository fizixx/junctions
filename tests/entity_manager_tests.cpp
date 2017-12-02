
#include "gtest/gtest.h"

#include "junctions/entity_manager.h"
#include "junctions/system_manager.h"

namespace ju {

struct MoveComponent {
  int x{0};
  int y{0};

  MoveComponent() = default;
  MoveComponent(int x, int y) : x(x), y(y) {}
};

struct AnotherComponent {
  int someValue{10};
};

TEST(EntityManagerTest, Basic) {
#if 0
  EntityManager em;

  Entity* entity1 = em.createEntity();

  entity1->addComponent<MoveComponent>(10, 20);
  ASSERT_TRUE(entity1->hasComponent<MoveComponent>());
  auto moveComp = entity1->getComponent<MoveComponent>();
  ASSERT_TRUE(moveComp != nullptr);
  EXPECT_EQ(10, moveComp->x);
  EXPECT_EQ(20, moveComp->y);

  auto anotherComp = entity1->getComponent<AnotherComponent>();
  EXPECT_TRUE(anotherComp == nullptr);
#endif  // 0
}

TEST(EntityManagerTest, Iteration) {
#if 0
  EntityManager em{nullptr};

  Entity* e1 = em.createEntity();
  e1->addComponent<MoveComponent>(10, 20);
  e1->addComponent<AnotherComponent>();

  Entity* e2 = em.createEntity();
  e2->addComponent<MoveComponent>(20, 30);

  for (auto& entity : em.allEntitiesWithComponent<MoveComponent>()) {
    EXPECT_TRUE(entity.hasComponent<MoveComponent>());
  }

  for (auto& entity : em.allEntitiesWithComponent<AnotherComponent>()) {
    EXPECT_TRUE(entity.hasComponent<AnotherComponent>());
  }

  for (auto& entity : em.allEntitiesWithComponent<MoveComponent, AnotherComponent>()) {
    EXPECT_TRUE(entity.hasComponent<MoveComponent>());
    EXPECT_TRUE(entity.hasComponent<AnotherComponent>());
  }
#endif  // 0
}

}  // namespace ju
