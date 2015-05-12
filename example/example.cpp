

#include "junctions/entity_manager.h"
#include "junctions/event_manager.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Event.hpp"

struct World {
  ju::EventManager events;
  ju::EntityManager entities;

  // World() : events{}, entities{&events} {}
};

int main(int argc, char* argv[]) {
  sf::RenderWindow window{sf::VideoMode{1600, 900}, "junctions - Example"};

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          window.close();
          break;
      }
    }

    window.clear(sf::Color{255, 0, 0, 255});
    window.display();
  }

  return 0;
}
