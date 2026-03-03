#include "engine.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

// ENGINE 3D
namespace
{
    constexpr float kNearPlane = 1.0f;

    static sf::Vector2f project(const Vec3 &p, int w, int h, float fov)
    {
        float z = p.z;
        if (z < kNearPlane)
            z = kNearPlane;
        float factor = fov / z;
        return {p.x * factor + w * 0.5f, -p.y * factor + h * 0.5f};
    }

    static Vec3 rotateX(const Vec3 &v, float a)
    {
        float c = std::cos(a), s = std::sin(a);
        return {v.x, v.y * c - v.z * s, v.y * s + v.z * c};
    }

    static Vec3 rotateY(const Vec3 &v, float a)
    {
        float c = std::cos(a), s = std::sin(a);
        return {v.x * c - v.z * s, v.y, v.x * s + v.z * c};
    }

    static bool clip_to_near_plane(Vec3 &a, Vec3 &b)
    {
        bool aVisible = a.z > kNearPlane;
        bool bVisible = b.z > kNearPlane;
        if (!aVisible && !bVisible)
            return false;
        if (aVisible && bVisible)
            return true;

        Vec3 behind = aVisible ? b : a;
        Vec3 inFront = aVisible ? a : b;
        float t = (kNearPlane - behind.z) / (inFront.z - behind.z);
        Vec3 clipped{
            behind.x + (inFront.x - behind.x) * t,
            behind.y + (inFront.y - behind.y) * t,
            kNearPlane};

        if (aVisible)
            b = clipped;
        else
            a = clipped;

        return true;
    }

    static bool load_debug_font(sf::Font &font)
    {
        static const char *paths[] = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
        };

        for (const char *path : paths)
        {
            if (font.loadFromFile(path))
                return true;
        }

        return false;
    }
}

// ENGINE KEYS
namespace
{
    sf::Keyboard::Key to_sf_key(aqwengine::InputKeys key)
    {
        switch (key)
        {
        case aqwengine::InputKeys::A_KEY:
            return sf::Keyboard::A;
        case aqwengine::InputKeys::B_KEY:
            return sf::Keyboard::B;
        case aqwengine::InputKeys::C_KEY:
            return sf::Keyboard::C;
        case aqwengine::InputKeys::D_KEY:
            return sf::Keyboard::D;
        case aqwengine::InputKeys::E_KEY:
            return sf::Keyboard::E;
        case aqwengine::InputKeys::F_KEY:
            return sf::Keyboard::F;
        case aqwengine::InputKeys::G_KEY:
            return sf::Keyboard::G;
        case aqwengine::InputKeys::H_KEY:
            return sf::Keyboard::H;
        case aqwengine::InputKeys::I_KEY:
            return sf::Keyboard::I;
        case aqwengine::InputKeys::J_KEY:
            return sf::Keyboard::J;
        case aqwengine::InputKeys::K_KEY:
            return sf::Keyboard::K;
        case aqwengine::InputKeys::L_KEY:
            return sf::Keyboard::L;
        case aqwengine::InputKeys::M_KEY:
            return sf::Keyboard::M;
        case aqwengine::InputKeys::N_KEY:
            return sf::Keyboard::N;
        case aqwengine::InputKeys::O_KEY:
            return sf::Keyboard::O;
        case aqwengine::InputKeys::P_KEY:
            return sf::Keyboard::P;
        case aqwengine::InputKeys::Q_KEY:
            return sf::Keyboard::Q;
        case aqwengine::InputKeys::R_KEY:
            return sf::Keyboard::R;
        case aqwengine::InputKeys::S_KEY:
            return sf::Keyboard::S;
        case aqwengine::InputKeys::T_KEY:
            return sf::Keyboard::T;
        case aqwengine::InputKeys::U_KEY:
            return sf::Keyboard::U;
        case aqwengine::InputKeys::V_KEY:
            return sf::Keyboard::V;
        case aqwengine::InputKeys::W_KEY:
            return sf::Keyboard::W;
        case aqwengine::InputKeys::X_KEY:
            return sf::Keyboard::X;
        case aqwengine::InputKeys::Y_KEY:
            return sf::Keyboard::Y;
        case aqwengine::InputKeys::Z_KEY:
            return sf::Keyboard::Z;
        default:
            return sf::Keyboard::Unknown;
        }
    }

    sf::Mouse::Button to_sf_mouse(aqwengine::MouseButton button)
    {
        switch (button)
        {
        case aqwengine::MouseButton::LEFT_MOUSE_BUTTON:
            return sf::Mouse::Left;
        case aqwengine::MouseButton::RIGHT_MOUSE_BUTTON:
            return sf::Mouse::Right;
        case aqwengine::MouseButton::MIDDLE_MOUSE_BUTTON:
            return sf::Mouse::Middle;
        default:
            return sf::Mouse::ButtonCount;
        }
    }
}

// Engine functions
void Engine::create_window(int w, int h, const std::string &title, bool vsync_enabled, int fps_limit)
{
    window.create(sf::VideoMode(w, h), title);
    if (vsync_enabled == true)
    {
        window.setVerticalSyncEnabled(true);
        fps_limit == 0 ? window.setFramerateLimit(60) : window.setFramerateLimit(fps_limit);
        cout << "[AQWENGINE] Framerate limit: " << fps_limit << " |" << "vsnyc is " << vsync_enabled << endl;
    }
    else
    {
        window.setVerticalSyncEnabled(false);
        fps_limit == 0 ? window.setFramerateLimit(0) : window.setFramerateLimit(fps_limit);
        cout << "[AQWENGINE] Framerate limit: " << fps_limit << " |" << "vsnyc is " << vsync_enabled << endl;
    }

    set_freecam_enabled(cam.enabled);
}

void Engine::clear_screen()
{
    window.clear();
}

void Engine::update_screen()
{
    draw_debug_overlay();
    window.display();
}

void Engine::poll_events()
{
    sf::Event e;
    while (window.pollEvent(e))
    {
        if (e.type == sf::Event::Closed)
            window.close();
    }
}

void Engine::destroy_window()
{
    cout << "[AQWENGINE] Destroying window..." << endl;
    window.close();
}

bool Engine::is_pressed_key(aqwengine::InputKeys key)
{
    return sf::Keyboard::isKeyPressed(to_sf_key(key));
}

bool Engine::is_pressed_mouse_button(aqwengine::MouseButton button)
{
    return sf::Mouse::isButtonPressed(to_sf_mouse(button));
}

bool Engine::is_open() const
{
    return window.isOpen();
}

sf::RenderWindow &Engine::get_window()
{
    return window;
}

void Engine::draw_line3d(float ax, float ay, float az,
                         float bx, float by, float bz,
                         sf::Color c)
{
    int w = (int)window.getSize().x;
    int h = (int)window.getSize().y;

    Vec3 a = to_view({ax, ay, az});
    Vec3 b = to_view({bx, by, bz});
    if (!clip_to_near_plane(a, b))
        return;

    auto pa = project(a, w, h, 400.f);
    auto pb = project(b, w, h, 400.f);

    sf::Vertex line[] = {sf::Vertex(pa, c), sf::Vertex(pb, c)};

    window.draw(line, 2, sf::Lines);
}

Vec3 Engine::to_view(Vec3 world) const
{
    // world - camPos
    Vec3 p{world.x - cam.x, world.y - cam.y, world.z - cam.z};

    // inverse camera rotation
    p = rotateY(p, -cam.yaw);
    p = rotateX(p, -cam.pitch);

    return p;
}

void Engine::draw_cube_wire(float cx, float cy, float cz, float size, float angleY)
{
    Vec3 pts[8] = {
        {cx - size, cy - size, cz - size},
        {cx + size, cy - size, cz - size},
        {cx + size, cy + size, cz - size},
        {cx - size, cy + size, cz - size},
        {cx - size, cy - size, cz + size},
        {cx + size, cy - size, cz + size},
        {cx + size, cy + size, cz + size},
        {cx - size, cy + size, cz + size},
    };

    // rotasyon + kamera uzaklığı
    for (auto &p : pts)
    {
        p = rotateY(p, angleY);
    }

    const int edges[12][2] = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

    int w = (int)window.getSize().x;
    int h = (int)window.getSize().y;

    sf::VertexArray lines(sf::Lines);

    for (int i = 0; i < 12; i++)
    {
        int a = edges[i][0];
        int b = edges[i][1];
        Vec3 va = to_view(pts[a]);
        Vec3 vb = to_view(pts[b]);
        if (!clip_to_near_plane(va, vb))
            continue;

        sf::Vector2f pa = project(va, w, h, 400.f);
        sf::Vector2f pb = project(vb, w, h, 400.f);

        lines.append(sf::Vertex(pa, sf::Color::White));
        lines.append(sf::Vertex(pb, sf::Color::White));
    }

    window.draw(lines);
}

void Engine::draw_grid3d(float size, float step)
{
    int w = (int)window.getSize().x;
    int h = (int)window.getSize().y;

    sf::VertexArray lines(sf::Lines);

    for (float i = -size; i <= size; i += step)
    {
        // Z sabit, X değişiyor
        Vec3 a = to_view({-size, 0.f, i});
        Vec3 b = to_view({size, 0.f, i});
        if (clip_to_near_plane(a, b))
        {
            auto pa = project(a, w, h, 400.f);
            auto pb = project(b, w, h, 400.f);
            lines.append(sf::Vertex(pa, sf::Color(80, 80, 80)));
            lines.append(sf::Vertex(pb, sf::Color(80, 80, 80)));
        }

        // X sabit, Z değişiyor
        Vec3 c = to_view({i, 0.f, -size});
        Vec3 d = to_view({i, 0.f, size});
        if (clip_to_near_plane(c, d))
        {
            auto pc = project(c, w, h, 400.f);
            auto pd = project(d, w, h, 400.f);
            lines.append(sf::Vertex(pc, sf::Color(80, 80, 80)));
            lines.append(sf::Vertex(pd, sf::Color(80, 80, 80)));
        }
    }

    window.draw(lines);
}

// Camera functions
void Engine::set_freecam_enabled(bool enabled)
{
    cam.enabled = enabled;
    cam.mouseLocked = false;

    window.setMouseCursorVisible(true);
    window.setMouseCursorGrabbed(false);

    auto p = sf::Mouse::getPosition(window);
    cam.lastMouseX = p.x;
    cam.lastMouseY = p.y;
}

void Engine::update_freecam(float dt, float moveSpeed, float mouseSens)
{
    if (!cam.enabled)
        return;

    if (dt > 0.05f)
        dt = 0.05f;

    bool wantsMouseLook = sf::Mouse::isButtonPressed(sf::Mouse::Right);
    if (wantsMouseLook != cam.mouseLocked)
    {
        cam.mouseLocked = wantsMouseLook;
        window.setMouseCursorVisible(true);
        window.setMouseCursorGrabbed(false);

        auto p = sf::Mouse::getPosition(window);
        cam.lastMouseX = p.x;
        cam.lastMouseY = p.y;
    }

    if (cam.mouseLocked)
    {
        auto p = sf::Mouse::getPosition(window);
        int dx = p.x - cam.lastMouseX;
        int dy = p.y - cam.lastMouseY;

        dx = std::clamp(dx, -40, 40);
        dy = std::clamp(dy, -40, 40);

        cam.yaw -= dx * mouseSens;
        cam.pitch += dy * mouseSens;

        const float limit = 1.55f;
        if (cam.pitch > limit)
            cam.pitch = limit;
        if (cam.pitch < -limit)
            cam.pitch = -limit;

        cam.lastMouseX = p.x;
        cam.lastMouseY = p.y;
    }

    float cy = std::cos(cam.yaw), sy = std::sin(cam.yaw);
    float cp = std::cos(cam.pitch), sp = std::sin(cam.pitch);

    Vec3 forward{-sy * cp, -sp, cy * cp};
    Vec3 right{cy, 0.f, sy};

    float v = moveSpeed * dt;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        cam.x += forward.x * v;
        cam.y += forward.y * v;
        cam.z += forward.z * v;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        cam.x -= forward.x * v;
        cam.y -= forward.y * v;
        cam.z -= forward.z * v;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        cam.x += right.x * v;
        cam.z += right.z * v;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        cam.x -= right.x * v;
        cam.z -= right.z * v;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        cam.y += v;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
        cam.y -= v;
}

void Engine::set_debug_coords_enabled(bool enabled)
{
    debugCoordsEnabled = enabled;
}

void Engine::draw_debug_overlay()
{
    if (!debugCoordsEnabled)
        return;

    if (!debugFontLoaded)
    {
        debugFontLoaded = load_debug_font(debugFont);
        if (!debugFontLoaded)
            return;
    }

    std::ostringstream textStream;
    textStream << std::fixed << std::setprecision(2)
               << "X: " << cam.x << '\n'
               << "Y: " << cam.y << '\n'
               << "Z: " << cam.z;

    sf::Text text;
    text.setFont(debugFont);
    text.setCharacterSize(16);
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1.f);
    text.setPosition(12.f, 8.f);
    text.setString(textStream.str());

    window.draw(text);
}
