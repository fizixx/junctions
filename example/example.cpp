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

#include <random>

#include "junctions/entity_manager.h"
#include "junctions/event_manager.h"
#include "junctions/system_manager.h"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/Shape.hpp"
#include "SFML/Window/Event.hpp"

struct Transformable {
  sf::Vector2f pos;

  Transformable(const sf::Vector2f& pos) : pos(pos) {}
};

struct Drawable {
  // The shape we draw.
  std::unique_ptr<sf::Shape> shape;

  // Whether the mouse is over this shape or not.
  bool isOver{false};

  Drawable(std::unique_ptr<sf::Shape> shape) : shape(std::move(shape)) {}

  void updatePosition(const sf::Vector2f& pos) {
    if (shape) {
      shape->setPosition(pos);
    }
  }

  void draw(sf::RenderTarget& target) {
    if (shape) {
      shape->setFillColor(isOver ? sf::Color{255, 0, 0} : sf::Color{0, 255, 0});
      target.draw(*shape);
    }
  }
};

struct DrawSystem {
  DrawSystem() = default;

  void update(ju::EntityManager& entities, ju::EventManager& events,
              float adjustment, sf::RenderTarget& target) {
    for (auto& entity :
         entities.allEntitiesWithComponent<Transformable, Drawable>()) {
      auto transformable = entity.getComponent<Transformable>();
      auto drawable = entity.getComponent<Drawable>();

      drawable->updatePosition(transformable->pos);
      drawable->draw(target);
    }
  }
};

struct EventSystem {
  EventSystem() = default;

  void update(ju::EntityManager& entities, ju::EventManager& events,
              sf::RenderTarget& target, sf::Event& event) {
    if (event.type == sf::Event::MouseMoved) {
      for (auto& entity : entities.allEntitiesWithComponent<Drawable>()) {
        // We use the drawable component to figure out the bounds of the entity.
        auto drawable = entity.getComponent<Drawable>();

        // Convert the mouse position to world position.
        sf::Vector2f mousePos = target.mapPixelToCoords(
            sf::Vector2i{event.mouseMove.x, event.mouseMove.y});

        // Get the bounds of the entity.
        sf::FloatRect bounds = drawable->shape->getGlobalBounds();

        // Update the state on the entity.
        drawable->isOver = bounds.contains(mousePos);
      }
    }
  }

  bool mouseIsDown{false};
};

struct World {
  ju::EventManager events;
  ju::EntityManager entities;
  ju::SystemManager systems;

  World()
    : events{}, entities{&events}, systems{&entities, &events} {
    systems.addSystem<EventSystem>();
    systems.addSystem<DrawSystem>();
  }

  void updateWithEvent(sf::RenderTarget& target, sf::Event& event) {
    systems.update<EventSystem>(target, event);
  }

  void update(float adjustment, sf::RenderTarget& target) {
    systems.update<DrawSystem>(adjustment, target);
  }

  void createEntities(size_t count) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<float> horDist(-500.f, 500.f);
    std::uniform_real_distribution<float> verDist(-300.f, 300.f);

    for (size_t i = 0; i < count; ++i) {
      auto entity = entities.createEntity();

      // Calculate a random position.
      sf::Vector2f pos{horDist(mt), verDist(mt)};
      entity->addComponent<Transformable>(pos);

      // Create a circle shape for the entity.
      auto shape = std::make_unique<sf::CircleShape>(30.f);
      sf::FloatRect bounds{shape->getLocalBounds()};
      shape->setOrigin(bounds.width / 2.f, bounds.height / 2.f);
      entity->addComponent<Drawable>(std::move(shape));
    }
  }
};

int main(int argc, char* argv[]) {
  sf::RenderWindow window{sf::VideoMode{1600, 900}, "junctions - Example"};
  auto windowSize = window.getSize();
  sf::View view{sf::FloatRect{-static_cast<float>(windowSize.x) / 2.f,
                              -static_cast<float>(windowSize.y) / 2.f,
                              static_cast<float>(windowSize.x),
                              static_cast<float>(windowSize.y)}};
  window.setView(view);

  World world;
  world.createEntities(10);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;

        default:
          world.updateWithEvent(window, event);
          break;
      }
    }

    window.clear(sf::Color{63, 63, 63, 255});
    world.update(1.f, window);
    window.display();
  }

  return 0;
}
