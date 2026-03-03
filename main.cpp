#include "engine.hpp"
#include <cmath>

int main()
{
    constexpr bool kShowDebugCoords = true;

    Engine e;
    e.create_window(800, 600, "aqwengine wire 3d", true, 60);
    e.set_debug_coords_enabled(kShowDebugCoords);

    float a = 0.f;
    sf::Clock clock;

    while (e.is_open())
    {
        float dt = clock.restart().asSeconds();

        e.poll_events();
        e.update_freecam(dt);

        e.clear_screen();
        e.draw_grid3d(30.f, 1.f);
        e.draw_cube_wire(0, 1, 0, 1.f, a);
        e.update_screen();
    }
}
