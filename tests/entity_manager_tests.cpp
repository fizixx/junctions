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
}

TEST(EntityManagerTest, Iteration) {
  EntityManager em;

  Entity* e1 = em.createEntity();
  e1->addComponent<MoveComponent>(10, 20);
  e1->addComponent<AnotherComponent>();

  Entity* e2 = em.createEntity();
  e2->addComponent<MoveComponent>(20, 30);

  for (auto& entity : em) {
    MoveComponent* moveComp = entity.getComponent<MoveComponent>();
    if (moveComp) {
      LOG(Info) << "moveComp: " << moveComp->x << ", " << moveComp->y;
    }

    AnotherComponent* anotherComp = entity.getComponent<AnotherComponent>();
    if (anotherComp) {
      LOG(Info) << "anotherComp: " << anotherComp->someValue;
    }
  }
}

}  // namespace ju
