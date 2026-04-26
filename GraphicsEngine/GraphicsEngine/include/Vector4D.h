#pragma once
#include "Prerequisites.h"
#include <string>
#include <sstream>
#include <cmath>

/**
 * @brief Vector de 4 componentes optimizado para DirectX 11.
 * alignas(16) asegura que sea compatible con las reglas de empaquetado de HLSL.
 */
class alignas(16) Vector4D
{
public:
    float x, y, z, w;

public:
    // Constructores
    Vector4D() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vector4D(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    Vector4D(const Vector4D& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}

    // Getters (para compatibilidad y seguridad const)
    float Getx() const { return x; }
    float Gety() const { return y; }
    float Getz() const { return z; }
    float Getw() const { return w; }

    // Setters
    void Setx(float value) { x = value; }
    void Sety(float value) { y = value; }
    void Setz(float value) { z = value; }
    void Setw(float value) { w = value; }

    // Establece todos los valores a la vez
    void Set(float _x, float _y, float _z, float _w) {
        x = _x; y = _y; z = _z; w = _w;
    }

    // --- OPERACIONES MATEMÁTICAS ---

    float Magnitude() const {
        return std::sqrt(x * x + y * y + z * z + w * w);
    }

    float SqrMagnitude() const {
        return (x * x + y * y + z * z + w * w);
    }

    void Normalize() {
        float m = Magnitude();
        if (m > 0.0f) {
            float invM = 1.0f / m;
            x *= invM; y *= invM; z *= invM; w *= invM;
        }
    }

    Vector4D Normalized() const {
        Vector4D res = *this;
        res.Normalize();
        return res;
    }

    float Dot(const Vector4D& other) const {
        return (x * other.x + y * other.y + z * other.z + w * other.w);
    }

    // --- SOBRECARGA DE OPERADORES ---

    Vector4D operator+(const Vector4D& rhs) const {
        return Vector4D(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
    }

    Vector4D operator-(const Vector4D& rhs) const {
        return Vector4D(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
    }

    Vector4D operator*(float scalar) const {
        return Vector4D(x * scalar, y * scalar, z * scalar, w * scalar);
    }

    Vector4D& operator*=(float scalar) {
        x *= scalar; y *= scalar; z *= scalar; w *= scalar;
        return *this;
    }

    bool operator==(const Vector4D& rhs) const {
        return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
    }

    // --- UTILIDADES ---

    static Vector4D Lerp(const Vector4D& a, const Vector4D& b, float t) {
        return a * (1.0f - t) + b * t;
    }

    std::string ToString() const {
        std::ostringstream os;
        os << "(" << x << ", " << y << ", " << z << ", " << w << ")";
        return os.str();
    }
};