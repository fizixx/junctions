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

#include "junctions/entity_manager.h"

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
  EntityManager em{nullptr};

  Entity entity1 = em.createEntity();
  Entity entity2 = em.createEntity();

  EXPECT_EQ(0, entity1.getId());
  EXPECT_EQ(1, entity2.getId());
}

TEST(EntityManagerTest, ComponentsThroughManager) {
  EntityManager em{nullptr};

  Entity entity1 = em.createEntity();

  {
    auto moveComponent = em.createComponent<MoveComponent>(entity1, 10, 20);
    EXPECT_EQ(10, moveComponent->x);
    EXPECT_EQ(20, moveComponent->y);

    EXPECT_TRUE(em.hasComponent<MoveComponent>(entity1));
    EXPECT_FALSE(em.hasComponent<AnotherComponent>(entity1));
  }

  Entity entity2 = em.createEntity();

  {
    auto moveComponent = em.createComponent<MoveComponent>(entity2, 20, 30);
    EXPECT_EQ(20, moveComponent->x);
    EXPECT_EQ(30, moveComponent->y);

    auto anotherComponent = em.createComponent<AnotherComponent>(entity2);
    EXPECT_EQ(10, anotherComponent->someValue);

    EXPECT_TRUE(em.hasComponent<MoveComponent>(entity2));
    EXPECT_TRUE(em.hasComponent<AnotherComponent>(entity2));
  }

  {
    auto moveComponent = em.getComponent<MoveComponent>(entity1);
    EXPECT_EQ(10, moveComponent->x);
    EXPECT_EQ(20, moveComponent->y);
  }
}

TEST(EntityManagerTest, ComponentsThroughEntity) {
  EntityManager em{nullptr};

  Entity entity1 = em.createEntity();

  {
    auto moveComponent = entity1.createComponent<MoveComponent>(10, 20);
    EXPECT_EQ(10, moveComponent->x);
    EXPECT_EQ(20, moveComponent->y);

    EXPECT_TRUE(entity1.hasComponent<MoveComponent>());
    EXPECT_FALSE(entity1.hasComponent<AnotherComponent>());
  }

  Entity entity2 = em.createEntity();

  {
    auto moveComponent = entity2.createComponent<MoveComponent>(20, 30);
    EXPECT_EQ(20, moveComponent->x);
    EXPECT_EQ(30, moveComponent->y);

    auto anotherComponent = entity2.createComponent<AnotherComponent>();
    EXPECT_EQ(10, anotherComponent->someValue);

    EXPECT_TRUE(entity2.hasComponent<MoveComponent>());
    EXPECT_TRUE(entity2.hasComponent<AnotherComponent>());
  }

  {
    auto moveComponent = entity1.getComponent<MoveComponent>();
    EXPECT_EQ(10, moveComponent->x);
    EXPECT_EQ(20, moveComponent->y);
  }
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

  for (auto& entity :
       em.allEntitiesWithComponent<MoveComponent, AnotherComponent>()) {
    EXPECT_TRUE(entity.hasComponent<MoveComponent>());
    EXPECT_TRUE(entity.hasComponent<AnotherComponent>());
  }
#endif  // 0
}

}  // namespace ju
