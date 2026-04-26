#pragma once
#include "Vector3D.h"

// Alineamos a 16 bytes para que la GPU procese cada punto de forma óptima
struct alignas(16) Point3D
{
    Vector3D position; // 1
    float padding;     // 2
    float r, g, b, a;  // 3, 4, 5, 6

    // El orden debe ser: position -> padding -> r -> g -> b -> a
    Point3D() : position(0, 0, 0), padding(0.0f), r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}

    Point3D(Vector3D pos, float red, float green, float blue)
        : position(pos), padding(0.0f), r(red), g(green), b(blue), a(1.0f) {
    }
};