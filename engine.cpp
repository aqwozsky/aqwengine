#include "engine.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

// 3D math and projection helpers used by the debug renderer.
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

    static Vec3 yaw_forward(float yaw)
    {
        return {-std::sin(yaw), 0.f, std::cos(yaw)};
    }

    static Vec3 yaw_right(float yaw)
    {
        return {std::cos(yaw), 0.f, std::sin(yaw)};
    }
}

// Input mapping helpers that keep SFML details out of the public API.
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
        case aqwengine::InputKeys::SPACE:
            return sf::Keyboard::Space;
        case aqwengine::InputKeys::ENTER:
            return sf::Keyboard::Enter;
        case aqwengine::InputKeys::ESCAPE:
            return sf::Keyboard::Escape;
        case aqwengine::InputKeys::BACKSPACE:
            return sf::Keyboard::Backspace;
        case aqwengine::InputKeys::TAB:
            return sf::Keyboard::Tab;
        case aqwengine::InputKeys::LEFT_SHIFT:
            return sf::Keyboard::LShift;
        case aqwengine::InputKeys::RIGHT_SHIFT:
            return sf::Keyboard::RShift;
        case aqwengine::InputKeys::LEFT_CONTROL:
            return sf::Keyboard::LControl;
        case aqwengine::InputKeys::RIGHT_CONTROL:
            return sf::Keyboard::RControl;
        case aqwengine::InputKeys::LEFT_ALT:
            return sf::Keyboard::LAlt;
        case aqwengine::InputKeys::RIGHT_ALT:
            return sf::Keyboard::RAlt;
        case aqwengine::InputKeys::F1:
            return sf::Keyboard::F1;
        case aqwengine::InputKeys::F2:
            return sf::Keyboard::F2;
        case aqwengine::InputKeys::F3:
            return sf::Keyboard::F3;
        case aqwengine::InputKeys::F4:
            return sf::Keyboard::F4;
        case aqwengine::InputKeys::F5:
            return sf::Keyboard::F5;
        case aqwengine::InputKeys::F6:
            return sf::Keyboard::F6;
        case aqwengine::InputKeys::F7:
            return sf::Keyboard::F7;
        case aqwengine::InputKeys::F8:
            return sf::Keyboard::F8;
        case aqwengine::InputKeys::F9:
            return sf::Keyboard::F9;
        case aqwengine::InputKeys::F10:
            return sf::Keyboard::F10;
        case aqwengine::InputKeys::F11:
            return sf::Keyboard::F11;
        case aqwengine::InputKeys::F12:
            return sf::Keyboard::F12;
        case aqwengine::InputKeys::INSERT:
            return sf::Keyboard::Insert;
        case aqwengine::InputKeys::DELETE:
            return sf::Keyboard::Delete;
        case aqwengine::InputKeys::HOME:
            return sf::Keyboard::Home;
        case aqwengine::InputKeys::END:
            return sf::Keyboard::End;
        case aqwengine::InputKeys::PAGE_UP:
            return sf::Keyboard::PageUp;
        case aqwengine::InputKeys::PAGE_DOWN:
            return sf::Keyboard::PageDown;
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

// Core window and rendering functions.
void Engine::create_window(int w, int h, const std::string &title, bool vsync_enabled, int fps_limit)
{
    window.create(sf::VideoMode(w, h), title);
    if (vsync_enabled == true)
    {
        window.setVerticalSyncEnabled(true);
        fps_limit == 0 ? window.setFramerateLimit(60) : window.setFramerateLimit(fps_limit);
        cout << "[AQWENGINE] Framerate limit: " << fps_limit << " |"
             << "vsync is " << vsync_enabled << endl;
    }
    else
    {
        window.setVerticalSyncEnabled(false);
        fps_limit == 0 ? window.setFramerateLimit(0) : window.setFramerateLimit(fps_limit);
        cout << "[AQWENGINE] Framerate limit: " << fps_limit << " |"
             << "vsync is " << vsync_enabled << endl;
    }

    set_freecam_enabled(cam.enabled);
}

void Engine::clear_screen()
{
    window.clear();
}

void Engine::update_screen()
{
    if (player.active)
    {
        draw_a_ball(player.position.x, player.position.y, player.position.z,
                    player.normal.x, player.normal.y, player.normal.z,
                    player.radius, player.color);

        Vec3 head = player.position + normalize(player.normal) * (player.radius * 1.8f);
        Vec3 facing = player.position + yaw_forward(player.yaw) * (player.radius * 1.8f);
        draw_line3d(player.position.x, player.position.y, player.position.z,
                    head.x, head.y, head.z,
                    sf::Color::Cyan);
        draw_line3d(player.position.x, player.position.y, player.position.z,
                    facing.x, facing.y, facing.z,
                    sf::Color::Green);
    }

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
    // Translate into camera space.
    Vec3 p{world.x - cam.x, world.y - cam.y, world.z - cam.z};

    // Apply the inverse camera rotation so the world is viewed from the camera.
    p = rotateY(p, -cam.yaw);
    p = rotateX(p, -cam.pitch);

    return p;
}

float Engine::get_player_support_y(float x, float z) const
{
    // Return the highest platform top under the player's X/Z position.
    float supportY = 0.f;

    for (const GroundBox &box : groundBoxes)
    {
        float minX = box.center.x - box.halfSize.x;
        float maxX = box.center.x + box.halfSize.x;
        float minZ = box.center.z - box.halfSize.z;
        float maxZ = box.center.z + box.halfSize.z;

        if (x < minX || x > maxX || z < minZ || z > maxZ)
            continue;

        float topY = box.center.y + box.halfSize.y;
        if (topY > supportY)
            supportY = topY;
    }

    return supportY;
}

void Engine::lock_cursor_to_center(bool enabled)
{
    if (enabled)
        window.setMouseCursorVisible(false);
    else
        window.setMouseCursorVisible(true);
}

void Engine::update_follow_camera()
{
    if (!followCam.enabled || !player.active)
        return;

    // Keep the camera behind the player while allowing mouse-driven orbit around the player.
    float orbitYaw = player.yaw + followCam.yawOffset;
    Vec3 forward = yaw_forward(orbitYaw);
    Vec3 cameraTarget = player.position + normalize(player.normal) * (player.radius * 0.75f);
    Vec3 cameraPos = cameraTarget - forward * followCam.distance + Vec3{0.f, followCam.height, 0.f};

    cam.x = cameraPos.x;
    cam.y = cameraPos.y;
    cam.z = cameraPos.z;
    cam.yaw = orbitYaw;
    cam.pitch = followCam.pitch;

    // Lock the mouse cursor to the center of the screen.
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

    // Rotate the cube in world space before projection.
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

void Engine::draw_a_ball(float x, float y, float z, float nx, float ny, float nz, float radius, sf::Color c)
{
    if (radius <= 0.f)
        return;

    constexpr float kPi = 3.14159265359f;
    constexpr int kSectors = 36;
    constexpr int kStacks = 18;

    Vec3 center{x, y, z};

    // Build a local basis so the sphere can be aligned to the supplied normal.
    Vec3 normal = (std::abs(nx) < 0.0001f && std::abs(ny) < 0.0001f && std::abs(nz) < 0.0001f)
                      ? Vec3{0.f, 1.f, 0.f}
                      : normalize({nx, ny, nz});
    Vec3 helper = std::abs(normal.y) > 0.99f ? Vec3{1.f, 0.f, 0.f} : Vec3{0.f, 1.f, 0.f};
    Vec3 tangent = normalize(cross(helper, normal));
    Vec3 bitangent = cross(normal, tangent);

    auto sphere_point = [&](float phi, float theta) -> Vec3
    {
        float sinPhi = std::sin(phi);
        float localX = radius * sinPhi * std::cos(theta);
        float localY = radius * sinPhi * std::sin(theta);
        float localZ = radius * std::cos(phi);
        return center + tangent * localX + bitangent * localY + normal * localZ;
    };

    std::vector<Vec3> previousRing(kSectors + 1);

    for (int stack = 0; stack <= kStacks; ++stack)
    {
        float phi = kPi * static_cast<float>(stack) / static_cast<float>(kStacks);
        std::vector<Vec3> currentRing(kSectors + 1);

        for (int sector = 0; sector <= kSectors; ++sector)
        {
            float theta = 2.f * kPi * static_cast<float>(sector) / static_cast<float>(kSectors);
            currentRing[sector] = sphere_point(phi, theta);

            if (sector > 0)
            {
                const Vec3 &a = currentRing[sector - 1];
                const Vec3 &b = currentRing[sector];
                draw_line3d(a.x, a.y, a.z, b.x, b.y, b.z, c);
            }

            if (stack > 0)
            {
                const Vec3 &a = previousRing[sector];
                const Vec3 &b = currentRing[sector];
                draw_line3d(a.x, a.y, a.z, b.x, b.y, b.z, c);
            }
        }

        previousRing = currentRing;
    }
}

void Engine::draw_grid3d(float size, float step)
{
    int w = (int)window.getSize().x;
    int h = (int)window.getSize().y;

    sf::VertexArray lines(sf::Lines);

    for (float i = -size; i <= size; i += step)
    {
        // Constant Z, varying X.
        Vec3 a = to_view({-size, 0.f, i});
        Vec3 b = to_view({size, 0.f, i});
        if (clip_to_near_plane(a, b))
        {
            auto pa = project(a, w, h, 400.f);
            auto pb = project(b, w, h, 400.f);
            lines.append(sf::Vertex(pa, sf::Color(80, 80, 80)));
            lines.append(sf::Vertex(pb, sf::Color(80, 80, 80)));
        }

        // Constant X, varying Z.
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

// Camera and player control functions.
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

void Engine::create_player(float x, float y, float z, float nx, float ny, float nz)
{
    player.active = true;
    player.position = {x, y, z};
    player.velocity = {0.f, 0.f, 0.f};
    player.yaw = 0.f;

    // A zero normal means "use world up".
    if (std::abs(nx) < 0.0001f && std::abs(ny) < 0.0001f && std::abs(nz) < 0.0001f)
        player.normal = {0.f, 1.f, 0.f};
    else
        player.normal = normalize({nx, ny, nz});

    lastPlayerSpawn.hasSpawn = true;
    lastPlayerSpawn.position = {x, y, z};
    lastPlayerSpawn.normal = player.normal;

    // Spawn the player on top of the supporting surface if needed.
    float minY = get_player_support_y(player.position.x, player.position.z) + player.radius;
    if (player.position.y <= minY)
    {
        player.position.y = minY;
        player.grounded = true;
    }
    else
    {
        player.grounded = false;
    }

    update_follow_camera();
}

void Engine::spawn_player()
{
    if (!lastPlayerSpawn.hasSpawn)
        return;

    spawn_player(lastPlayerSpawn.position.x, lastPlayerSpawn.position.y, lastPlayerSpawn.position.z,
                 lastPlayerSpawn.normal.x, lastPlayerSpawn.normal.y, lastPlayerSpawn.normal.z);
}

void Engine::spawn_player(float x, float y, float z, float nx, float ny, float nz)
{
    create_player(x, y, z, nx, ny, nz);
    set_follow_camera_enabled(true);
}

void Engine::add_player(float x, float y, float z, float nx, float ny, float nz)
{
    create_player(x, y, z, nx, ny, nz);
}

void Engine::remove_player(bool keepLastSpawn)
{
    go_back_to_freecam_from_player();
    if (!keepLastSpawn)
        lastPlayerSpawn.hasSpawn = false;
    player.active = false;
    player.velocity = {0.f, 0.f, 0.f};
    player.grounded = false;
}

void Engine::set_player_position(float x, float y, float z)
{
    if (!player.active)
        return;

    player.position = {x, y, z};

    float minY = get_player_support_y(player.position.x, player.position.z) + player.radius;
    if (player.position.y <= minY)
    {
        player.position.y = minY;
        player.velocity.y = 0.f;
        player.grounded = true;
    }
    else
    {
        player.grounded = false;
    }

    update_follow_camera();
}

void Engine::go_back_to_freecam_from_player()
{
    if (player.active)
    {
        float orbitYaw = player.yaw + followCam.yawOffset;
        Vec3 forward = yaw_forward(orbitYaw);
        Vec3 freecamPos = player.position - forward * followCam.distance + Vec3{0.f, followCam.height, 0.f};

        cam.x = freecamPos.x;
        cam.y = freecamPos.y;
        cam.z = freecamPos.z;
        cam.yaw = orbitYaw;
        cam.pitch = followCam.pitch;
    }

    followCam.enabled = false;
    set_freecam_enabled(true);
}

void Engine::move_player(float dx, float dy, float dz)
{
    if (!player.active)
        return;

    // Movement is in local player space: X = strafe, Z = forward/back.
    // In follow camera mode, movement is interpreted relative to the current camera orbit.
    float movementYaw = followCam.enabled ? (player.yaw + followCam.yawOffset) : player.yaw;
    if (followCam.enabled && (std::abs(dx) > 0.0001f || std::abs(dz) > 0.0001f))
    {
        player.yaw = movementYaw;
        followCam.yawOffset = 0.f;
    }

    if (followCam.enabled && movementYaw != cam.yaw)
    {
        this->move_player(cam.yaw - movementYaw, 0.f, 0.f);
    }
    // The incoming values are treated as intent scaled by frame time, then expanded by moveSpeed.
    Vec3 right = yaw_right(movementYaw);
    Vec3 forward = yaw_forward(movementYaw);
    Vec3 planarMove = (right * dx + forward * dz) * player.moveSpeed;

    player.position.x += planarMove.x;
    player.position.z += planarMove.z;
    player.position.y += dy * player.moveSpeed;

    // While grounded, keep the player snapped to the support surface.
    if (player.grounded)
    {
        float minY = get_player_support_y(player.position.x, player.position.z) + player.radius;
        player.position.y = std::max(player.position.y, minY);
    }

    update_follow_camera();
}

void Engine::rotate_player(float deltaYaw)
{
    if (!player.active)
        return;

    player.yaw += deltaYaw;
    update_follow_camera();
}

void Engine::jump_player(float impulse)
{
    if (!player.active || !player.grounded)
        return;

    player.velocity.y = impulse;
    player.grounded = false;
}

void Engine::update_player(float dt)
{
    if (!player.active)
        return;

    if (dt > 0.05f)
        dt = 0.05f;

    // Basic vertical physics: gravity pulls down, then the player lands on the top-most platform.
    player.velocity.y -= player.gravity * dt;
    player.position.y += player.velocity.y * dt;

    float minY = get_player_support_y(player.position.x, player.position.z) + player.radius;
    if (player.position.y <= minY)
    {
        player.position.y = minY;
        player.velocity.y = 0.f;
        player.grounded = true;
    }
    else
    {
        player.grounded = false;
    }

    update_follow_camera();
}

void Engine::set_player_rotation(float yaw)
{
    if (!player.active)
        return;

    player.yaw = yaw;
    update_follow_camera();
}

float Engine::get_player_rotation() const
{
    return player.yaw;
}

Vec3 Engine::get_player_position() const
{
    return player.position;
}

Engine::PlayerInfo Engine::get_player() const
{
    return {
        player.active,
        player.position,
        player.normal,
        player.velocity,
        player.radius,
        player.yaw,
        player.moveSpeed,
        player.grounded,
    };
}

bool Engine::get_player_grounded() const
{
    return player.grounded;
}

void Engine::set_player_move_speed(float speed)
{
    player.moveSpeed = std::max(0.f, speed);
}

void Engine::set_follow_camera_enabled(bool enabled)
{
    followCam.enabled = enabled;
    if (enabled)
        update_follow_camera();
}

void Engine::set_follow_camera_offset(float distance, float height)
{
    followCam.distance = distance;
    followCam.height = height;
    update_follow_camera();
}

void Engine::set_camlock_player_movement(bool enabled)
{
    camlockPlayerMovement = enabled;

    if (enabled && player.active && followCam.enabled)
    {
        player.yaw += followCam.yawOffset;
        followCam.yawOffset = 0.f;
        update_follow_camera();
    }
}

bool Engine::get_camlock_player_movement() const
{
    return camlockPlayerMovement;
}

void Engine::clear_ground_colliders()
{
    groundBoxes.clear();
}

void Engine::add_ground_box(float cx, float cy, float cz, float halfX, float halfY, float halfZ)
{
    // Ground boxes currently act as "walkable top surfaces" for the player.
    groundBoxes.push_back({{cx, cy, cz}, {std::abs(halfX), std::abs(halfY), std::abs(halfZ)}});
}

void Engine::update_freecam(float dt, float moveSpeed, float mouseSens)
{
    if (followCam.enabled && player.active)
    {
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

            if (camlockPlayerMovement)
                player.yaw -= dx * mouseSens;
            else
                followCam.yawOffset -= dx * mouseSens;
            followCam.pitch += dy * mouseSens;

            const float limit = 1.2f;
            if (followCam.pitch > limit)
                followCam.pitch = limit;
            if (followCam.pitch < -0.7f)
                followCam.pitch = -0.7f;

            cam.lastMouseX = p.x;
            cam.lastMouseY = p.y;
        }

        update_follow_camera();
        return;
    }

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
               << "Camera: " << ((followCam.enabled && player.active) ? "followcam" : "freecam") << '\n'
               << "X: " << cam.x << '\n'
               << "Y: " << cam.y << '\n'
               << "Z: " << cam.z;

    if (player.active)
    {
        textStream << '\n'
                   << "PX: " << player.position.x << '\n'
                   << "PY: " << player.position.y << '\n'
                   << "PZ: " << player.position.z << '\n'
                   << "Yaw: " << player.yaw << '\n'
                   << "MoveSpeed: " << player.moveSpeed << '\n'
                   << "Grounded: " << (player.grounded ? "true" : "false");
    }
    else
    {
        textStream << '\n'
                   << "Player: inactive";
    }

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

void Engine::set_to_first_person(bool enabled, bool lockMouse)
{
    if (!enabled && !player.active)
        return;
    else if (player.active && !lockMouse)
    {
        cam.y = player.position.y;
        cam.x = player.position.x;
        cam.z = player.position.z;
        cam.yaw = player.yaw;
        cam.pitch = 0.f;
    }
    else if (player.active && lockMouse)
    {
        cam.y = player.position.y;
        cam.x = player.position.x;
        cam.z = player.position.z;
        cam.yaw = player.yaw;
        cam.pitch = 0.f;
        this->lock_cursor_to_center(true);
    }
}
