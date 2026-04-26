#pragma once
#include "Prerequisites.h"
#include "Vector2D.h"

//Matriz 2x2 usada para transformaciones en 2D

class Matrix2x2
{
public:
    // Elementos de la matriz
    // [ m00 m01 ]
    // [ m10 m11 ]
    float m00, m01;
    float m10, m11;

    // Constructor identidad por defecto
    Matrix2x2()
        : m00(1), m01(0),
        m10(0), m11(1) {
    }

    // Constructor principal
    Matrix2x2(float _m00, float _m01,
        float _m10, float _m11)
        : m00(_m00), m01(_m01),
        m10(_m10), m11(_m11) {
    }

    // Constructor copia
    Matrix2x2(const Matrix2x2& other)
        : m00(other.m00), m01(other.m01),
        m10(other.m10), m11(other.m11) {
    }

    //Métodos estáticos

    // Matriz identidad
    static Matrix2x2 Identity() {
        return Matrix2x2();
    }

    // Matriz de rotación (radianes)
    static Matrix2x2 Rotation(float radians) {
        float c = std::cos(radians);
        float s = std::sin(radians);

        return Matrix2x2(
            c, -s,
            s, c
        );
    }

    // Matriz de escala
    static Matrix2x2 Scale(float sx, float sy) {
        return Matrix2x2(
            sx, 0,
            0, sy
        );
    }

    //Operadores

    // Multiplicación matriz * matriz
    Matrix2x2 operator*(const Matrix2x2& rhs) const {
        return Matrix2x2(
            m00 * rhs.m00 + m01 * rhs.m10,
            m00 * rhs.m01 + m01 * rhs.m11,

            m10 * rhs.m00 + m11 * rhs.m10,
            m10 * rhs.m01 + m11 * rhs.m11
        );
    }

    // Multiplicación matriz * vector
    Vector2D operator*(const Vector2D& v) const {
        return Vector2D(
            m00 * v.GetX() + m01 * v.GetY(),
            m10 * v.GetX() + m11 * v.GetY()
        );
    }

    //Utilidades

    std::string ToString() const {
        std::ostringstream os;
        os << "[[" << m00 << ", " << m01 << "], "
            << "[" << m10 << ", " << m11 << "]]";
        return os.str();
    }
};