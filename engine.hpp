#pragma once
#include <SFML/Graphics.hpp>
#include "math3d.hpp"
#include <string>
#include <vector>

namespace aqwengine
{
    enum class InputKeys
    {
        A_KEY,
        B_KEY,
        C_KEY,
        D_KEY,
        E_KEY,
        F_KEY,
        G_KEY,
        H_KEY,
        I_KEY,
        J_KEY,
        K_KEY,
        L_KEY,
        M_KEY,
        N_KEY,
        O_KEY,
        P_KEY,
        Q_KEY,
        R_KEY,
        S_KEY,
        T_KEY,
        U_KEY,
        V_KEY,
        W_KEY,
        X_KEY,
        Y_KEY,
        Z_KEY,

        UP_ARROW,
        DOWN_ARROW,
        LEFT_ARROW,
        RIGHT_ARROW,

        NUMPAD_0,
        NUMPAD_1,
        NUMPAD_2,
        NUMPAD_3,
        NUMPAD_4,
        NUMPAD_5,
        NUMPAD_6,
        NUMPAD_7,
        NUMPAD_8,
        NUMPAD_9,

        BACKSPACE,
        DELETE,
        ENTER,
        ESCAPE,
        LEFT_SHIFT,
        RIGHT_SHIFT,
        LEFT_CONTROL,
        RIGHT_CONTROL,
        LEFT_ALT,
        RIGHT_ALT,
        PAGE_UP,
        PAGE_DOWN,
        HOME,
        END,
        INSERT,
        SPACE,
        TAB,

        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
    };

    enum class MouseButton
    {
        LEFT_MOUSE_BUTTON,
        RIGHT_MOUSE_BUTTON,
        MIDDLE_MOUSE_BUTTON,
    };
}

class Engine
{
public:
    struct PlayerInfo
    {
        bool active = false;
        Vec3 position{0.f, 0.f, 0.f};
        Vec3 normal{0.f, 1.f, 0.f};
        Vec3 velocity{0.f, 0.f, 0.f};
        float radius = 0.f;
        float yaw = 0.f;
        float moveSpeed = 0.f;
        bool grounded = false;
    };

    void create_window(int w, int h, const std::string &title, bool vsync_enabled, int fps_limit);
    void clear_screen();
    void update_screen();
    bool is_pressed_key(aqwengine::InputKeys key);
    bool is_pressed_mouse_button(aqwengine::MouseButton button);
    bool is_open() const;
    void poll_events();
    void destroy_window();

    // 3D debug (wireframe)
    sf::RenderWindow &get_window();
    void draw_line3d(float ax, float ay, float az,
                     float bx, float by, float bz,
                     sf::Color c = sf::Color::White);
    void draw_cube_wire(float cx, float cy, float cz, float size, float angleY);
    void draw_grid3d(float size, float step);
    void set_freecam_enabled(bool enabled);
    void update_freecam(float dt, float moveSpeed = 60.0f, float mouseSens = 0.01f);
    void set_debug_coords_enabled(bool enabled);
    void draw_a_ball(float x, float y, float z, float nx, float ny, float nz, float radius, sf::Color c = sf::Color::White);
    void create_player(float x, float y, float z, float nx, float ny, float nz);
    void spawn_player();
    void spawn_player(float x, float y, float z, float nx, float ny, float nz);
    void add_player(float x, float y, float z, float nx, float ny, float nz); // Backward-compatible alias
    void remove_player(bool keepLastSpawn = true);
    void set_player_position(float x, float y, float z);
    void move_player(float dx, float dy, float dz);
    void rotate_player(float deltaYaw);
    void jump_player(float impulse = 6.0f);
    void update_player(float dt);
    void set_player_rotation(float yaw);
    float get_player_rotation() const;
    Vec3 get_player_position() const;
    PlayerInfo get_player() const;
    bool get_player_grounded() const;
    void set_player_move_speed(float speed);
    void set_follow_camera_enabled(bool enabled);
    void set_follow_camera_offset(float distance, float height);
    void set_camlock_player_movement(bool enabled);
    bool get_camlock_player_movement() const;
    void clear_ground_colliders();
    void add_ground_box(float cx, float cy, float cz, float halfX, float halfY, float halfZ);
    void go_back_to_freecam_from_player();
    void set_to_first_person(bool enabled, bool lockMouse);
    void lock_cursor_to_center(bool enabled);

private:
    sf::RenderWindow window;
    struct CamState
    {
        float x = 0.f, y = 1.f, z = -5.f;
        float yaw = 0.f;
        float pitch = 0.f;
        bool enabled = true;
        bool mouseLocked = false;
        int lastMouseX = 0, lastMouseY = 0;
    } cam;

    bool debugCoordsEnabled = false;
    sf::Font debugFont;
    bool debugFontLoaded = false;

    struct PlayerState
    {
        bool active = false;
        Vec3 position{0.f, 0.f, 0.f};
        Vec3 normal{0.f, 1.f, 0.f};
        Vec3 velocity{0.f, 0.f, 0.f};
        float radius = 0.45f;
        float yaw = 0.f;
        float moveSpeed = 4.0f;
        float gravity = 16.0f;
        float groundY = 0.f;
        bool grounded = false;
        sf::Color color = sf::Color(255, 220, 120);
    } player;

    struct PlayerSpawnState
    {
        bool hasSpawn = false;
        Vec3 position{0.f, 0.f, 0.f};
        Vec3 normal{0.f, 1.f, 0.f};
    } lastPlayerSpawn;

    struct FollowCameraState
    {
        bool enabled = false;
        float distance = 6.f;
        float height = 2.5f;
        float yawOffset = 0.f;
        float pitch = 0.35f;
    } followCam;

    bool camlockPlayerMovement = false;

    struct GroundBox
    {
        Vec3 center{0.f, 0.f, 0.f};
        Vec3 halfSize{0.f, 0.f, 0.f};
    };

    std::vector<GroundBox> groundBoxes;

    Vec3 to_view(Vec3 world) const;
    float get_player_support_y(float x, float z) const;
    void update_follow_camera();
    void draw_debug_overlay();
};
