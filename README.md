# aqwengine

`aqwengine` is a small C++/SFML playground for simple 3D wireframe rendering, free camera movement, and basic player controller experiments.

## Features

- SFML window creation and frame updates
- Keyboard and mouse input helpers
- Debug 3D line drawing
- Wireframe cube rendering
- 3D grid / baseplate rendering
- Free camera movement with `W`, `A`, `S`, `D`, `Space`, and `Left Shift`
- Right mouse drag camera look
- Optional on-screen camera `X`, `Y`, `Z` debug overlay
- Wireframe sphere rendering
- Basic player spawning, rotation, movement, and jumping
- Follow camera mode
- Ground box support surfaces for simple platforming

## Build

This project uses `make` and `g++`.

Requirements:

- C++20 compiler
- SFML graphics, window, and system libraries

Build the app:

```bash
make
```

Build and run:

```bash
make run
```

Clean build output:

```bash
make clean
```

## Controls

- `Right Mouse Button`: drag to look around
- `Right Mouse Button` while follow camera is active: orbit the camera around the player
- `W` / `A` / `S` / `D`: move the player while follow camera is active, or move the camera in freecam
- `Space` / `Left Shift`: move the camera up / down in freecam
- `Q` / `E`: rotate player left / right
- `Space`: jump while a player is active
- `R`: reset player to a test position
- `F`: leave follow camera and return to freecam
- `G`: remove the player and automatically restore freecam
- `H`: respawn the player using the last remembered spawn settings

## Player Notes

- `move_player(dx, dy, dz)` moves in local player space, not world space
- While follow camera is active, player movement is aligned to the current camera orbit
- Ground boxes currently behave as walkable top surfaces only
- Follow camera overrides the free camera update while it is enabled
- Right mouse drag still works in follow camera mode and adjusts orbit yaw/pitch

## Player And Camera API

- `create_player(...)`: spawn a player and initialize its facing/up vectors
- `spawn_player()`: respawn the most recently remembered player spawn
- `spawn_player(x, y, z, nx, ny, nz)`: spawn immediately and refresh the remembered spawn
- `remove_player(bool keepLastSpawn = true)`: delete the player, restore freecam, and optionally forget the remembered spawn
- `set_player_position(...)`: teleport the player
- `move_player(...)`: move in local player space
- `rotate_player(...)` / `set_player_rotation(...)`: change facing direction
- `jump_player(...)`: apply an upward jump impulse when grounded
- `update_player(dt)`: advance gravity and landing logic
- `get_player_position()`: read the current player position
- `get_player_rotation()`: read the current player yaw
- `get_player_grounded()`: check whether the player is standing on a support surface
- `set_player_move_speed(...)`: control movement speed scaling
- `set_follow_camera_enabled(...)`: toggle follow camera mode
- `set_follow_camera_offset(...)`: control follow camera distance and height
- `set_camlock_player_movement(...)`: toggle whether right-click camera rotation also rotates the player
- `get_camlock_player_movement()`: read the current camlock toggle state
- `go_back_to_freecam_from_player()`: keep the current player-relative view, then hand control back to freecam
- `clear_ground_colliders()` / `add_ground_box(...)`: define simple walkable support surfaces

## Debug Overlay

- Shows the current camera mode as `freecam` or `followcam`
- Shows free camera position at all times
- Shows player position, yaw, move speed, and grounded state while a player is active
- Shows `Player: inactive` after the player has been removed

## API Example

```cpp
Engine e;
e.create_window(800, 600, "sample", true, 60);

e.clear_ground_colliders();
e.add_ground_box(0.f, -0.5f, 0.f, 20.f, 0.5f, 20.f);

e.create_player(0.f, 0.45f, 0.f, 0.f, 1.f, 0.f);
e.set_camlock_player_movement(true);
e.set_follow_camera_enabled(true);

e.remove_player();                 // Returns control to freecam and keeps the remembered spawn
e.spawn_player();                  // Respawns at the remembered player spawn
e.spawn_player(4.f, 2.f, 1.f, 0.f, 1.f, 0.f); // Overrides the remembered spawn
e.remove_player(false);            // Removes the player and clears the remembered spawn
```

## Debug Coordinates Overlay

The sample app enables the coordinate overlay in [`main.cpp`](/home/aqwoz/aqwengine/main.cpp):

```cpp
constexpr bool kShowDebugCoords = true;
```

Set it to `false` if you want the overlay disabled.

## Project Layout

- [`engine.hpp`](/home/aqwoz/aqwengine/engine.hpp): engine interface
- [`engine.cpp`](/home/aqwoz/aqwengine/engine.cpp): engine implementation, debug 3D rendering, player logic, and camera control
- [`main.cpp`](/home/aqwoz/aqwengine/main.cpp): sample application
- [`math3d.hpp`](/home/aqwoz/aqwengine/math3d.hpp): small vector math helpers
- [`renderer3d.cpp`](/home/aqwoz/aqwengine/renderer3d.cpp): additional renderer code

## License

This project is licensed under the MIT License. See [`LICENSE`](/home/aqwoz/aqwengine/LICENSE).
