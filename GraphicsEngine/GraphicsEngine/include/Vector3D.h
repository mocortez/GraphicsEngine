#pragma once
#include "Prerequisites.h"
#include <string>
#include <sstream>
#include <cmath>

/**
 * @brief Vector de 3 componentes (XYZ).
 * Usado principalmente para datos de vértices (posiciones, normales).
 * Nota: Para Constant Buffers, se recomienda usar Vector4D o añadir padding.
 */
class Vector3D
{
public:
    float x, y, z;

public:
    // Constructores
    Vector3D() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3D(float x, float y, float z) : x(x), y(y), z(z) {}
    Vector3D(const Vector3D& other) : x(other.x), y(other.y), z(other.z) {}

    // Getters
    float Getx() const { return x; }
    float Gety() const { return y; }
    float Getz() const { return z; }

    // Setters
    void Setx(float value) { x = value; }
    void Sety(float value) { y = value; }
    void Setz(float value) { z = value; }

    void Set(float _x, float _y, float _z) {
        x = _x; y = _y; z = _z;
    }

    // --- OPERACIONES MATEMÁTICAS ---

    float Magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    float SqrMagnitude() const {
        return (x * x + y * y + z * z);
    }

    void Normalize() {
        float m = Magnitude();
        if (m > 0.0f) {
            float invM = 1.0f / m;
            x *= invM; y *= invM; z *= invM;
        }
    }

    Vector3D Normalized() const {
        Vector3D res = *this;
        res.Normalize();
        return res;
    }

    float Dot(const Vector3D& other) const {
        return (x * other.x + y * other.y + z * other.z);
    }

    // Producto Cruz (Crucial para generar normales/tangentes en Tech Art)
    static Vector3D Cross(const Vector3D& v1, const Vector3D& v2) {
        return Vector3D(
            v1.y * v2.z - v1.z * v2.y,
            v1.z * v2.x - v1.x * v2.z,
            v1.x * v2.y - v1.y * v2.x
        );
    }

    // --- SOBRECARGA DE OPERADORES ---

    Vector3D operator+(const Vector3D& rhs) const {
        return Vector3D(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    Vector3D operator-(const Vector3D& rhs) const {
        return Vector3D(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    Vector3D operator*(float scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }

    Vector3D& operator*=(float scalar) {
        x *= scalar; y *= scalar; z *= scalar;
        return *this;
    }

    bool operator==(const Vector3D& rhs) const {
        return (x == rhs.x && y == rhs.y && z == rhs.z);
    }

    // --- UTILIDADES ---

    static Vector3D Lerp(const Vector3D& a, const Vector3D& b, float t) {
        return a * (1.0f - t) + b * t;
    }

    std::string ToString() const {
        std::ostringstream os;
        os << "(" << x << ", " << y << ", " << z << ")";
        return os.str();
    }
};
