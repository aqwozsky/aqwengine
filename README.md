# aqwengine

`aqwengine` is a small C++/SFML playground for simple 3D wireframe rendering and free camera movement.

## Features

- SFML window creation and frame updates
- Keyboard and mouse input helpers
- Debug 3D line drawing
- Wireframe cube rendering
- 3D grid / baseplate rendering
- Free camera movement with `W`, `A`, `S`, `D`, `Space`, and `Left Shift`
- Right mouse drag camera look
- Optional on-screen camera `X`, `Y`, `Z` debug overlay

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
- `W` / `A` / `S` / `D`: move camera
- `Space`: move up
- `Left Shift`: move down

## Debug Coordinates Overlay

The sample app enables the coordinate overlay in [`main.cpp`](/home/aqwoz/aqwengine/main.cpp):

```cpp
constexpr bool kShowDebugCoords = true;
```

Set it to `false` if you want the overlay disabled.

## Project Layout

- [`engine.hpp`](/home/aqwoz/aqwengine/engine.hpp): engine interface
- [`engine.cpp`](/home/aqwoz/aqwengine/engine.cpp): engine implementation and debug 3D rendering
- [`main.cpp`](/home/aqwoz/aqwengine/main.cpp): sample application
- [`math3d.hpp`](/home/aqwoz/aqwengine/math3d.hpp): small vector math helpers
- [`renderer3d.cpp`](/home/aqwoz/aqwengine/renderer3d.cpp): additional renderer code

## License

This project is licensed under the MIT License. See [`LICENSE`](/home/aqwoz/aqwengine/LICENSE).
