#pragma once
#include <SFML/Graphics.hpp>
#include "math3d.hpp"
#include <string>

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

    Vec3 to_view(Vec3 world) const; // private helper
    void draw_debug_overlay();
};
