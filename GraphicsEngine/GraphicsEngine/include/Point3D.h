#pragma once
#include "Vector3D.h"

struct MeshUV {
    float u, v;
    MeshUV() : u(0.0f), v(0.0f) {}
    MeshUV(float _u, float _v) : u(_u), v(_v) {}
};

// Alineación estricta de 32 bytes exactos (12 + 12 + 8) sin paddings ocultos
struct alignas(16) Point3D
{
    Vector3D position; // 12 bytes (offset: 0)
    Vector3D normal;   // 12 bytes (offset: 12)
    MeshUV texcoord;   // 8 bytes  (offset: 24)

    Point3D() : position(0, 0, 0), normal(0, 1, 0), texcoord(0, 0) {}

    Point3D(Vector3D pos, Vector3D norm, MeshUV uv)
        : position(pos), normal(norm), texcoord(uv) {
    }
};