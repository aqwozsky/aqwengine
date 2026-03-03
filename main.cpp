#include "engine.hpp"
#include <cmath>

int main()
{
    constexpr bool kShowDebugCoords = true;
    constexpr float kPlayerBaseSpeed = 4.0f;

    Engine e;
    e.create_window(800, 600, "aqwengine wire 3d", true, 60);
    e.set_debug_coords_enabled(kShowDebugCoords);

    // Build two walkable surfaces: a large floor and a raised platform.
    e.clear_ground_colliders();
    e.add_ground_box(0.f, -0.5f, 0.f, 30.f, 0.5f, 30.f);
    e.add_ground_box(6.f, 1.f, 6.f, 3.f, 0.35f, 3.f);

    // Spawn the player, aim it slightly to the right, then enable follow camera.
    e.create_player(2.f, 0.45f, 4.f, 0.f, 1.f, 0.f);
    Engine::PlayerInfo player = e.get_player();
    e.set_player_rotation(0.35f);
    e.set_player_move_speed(kPlayerBaseSpeed);
    e.set_follow_camera_offset(7.f, 3.f);
    e.set_camlock_player_movement(true);
    e.set_follow_camera_enabled(true);

    float a = 0.f;
    sf::Clock clock;

    while (e.is_open())
    {
        float dt = clock.restart().asSeconds();
        float playerStep = dt;
        float playerTurn = 1.8f * dt;

        e.poll_events();
        e.update_freecam(dt);

        // WASD moves the player; in follow camera mode movement is camera-relative.
        if (e.is_pressed_key(aqwengine::InputKeys::W_KEY))
            e.move_player(0.f, 0.f, playerStep);
        if (e.is_pressed_key(aqwengine::InputKeys::S_KEY))
            e.move_player(0.f, 0.f, -playerStep);
        if (e.is_pressed_key(aqwengine::InputKeys::A_KEY))
            e.move_player(-playerStep, 0.f, 0.f);
        if (e.is_pressed_key(aqwengine::InputKeys::D_KEY))
            e.move_player(playerStep, 0.f, 0.f);
        if (e.is_pressed_key(aqwengine::InputKeys::Q_KEY))
            e.rotate_player(-playerTurn);
        if (e.is_pressed_key(aqwengine::InputKeys::E_KEY))
            e.rotate_player(playerTurn);
        if (e.is_pressed_key(aqwengine::InputKeys::R_KEY))
            e.set_player_position(2.f, 3.f, 4.f);
        if (e.is_pressed_key(aqwengine::InputKeys::F_KEY))
            e.go_back_to_freecam_from_player();
        if (e.is_pressed_key(aqwengine::InputKeys::G_KEY))
            e.remove_player();
        if (e.is_pressed_key(aqwengine::InputKeys::H_KEY))
            e.spawn_player();
        if (e.is_pressed_key(aqwengine::InputKeys::SPACE))
            e.jump_player();

        player = e.get_player();

        // Reduce air control slightly so the grounded accessor has an obvious use in the sample.
        if (e.get_player_grounded())
            e.set_player_move_speed(kPlayerBaseSpeed);
        else
            e.set_player_move_speed(kPlayerBaseSpeed * 0.8f);

        e.update_player(dt);
        e.set_to_first_person(true, true);

        e.clear_screen();
        e.draw_grid3d(30.f, 1.f);
        e.draw_cube_wire(0, 1, 0, 1.f, a);
        e.draw_cube_wire(6.f, 1.f, 6.f, 3.f, 0.f);
        e.draw_a_ball(0, 1, 0, 0, 1, 0, 1.f);
        e.update_screen();
    }
}
