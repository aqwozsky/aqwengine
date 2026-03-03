#pragma once
#include "math3d.hpp"

struct Camera
{
    Vec3 position{0, 0, -5};
    float fov = 90.f;
    float nearPlane = 0.1f;
    float farPlane = 100.f;
};