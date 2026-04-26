#pragma once
#include "Prerequisites.h"
#include "Vector3D.h"

//@brief Matriz 3x3 para transformaciones 3D (sin traslación)
 
class Matrix3x3
{
public:
    // [ m00 m01 m02 ]
    // [ m10 m11 m12 ]
    // [ m20 m21 m22 ]
    float m00, m01, m02;
    float m10, m11, m12;
    float m20, m21, m22;

    // Identidad
    Matrix3x3()
        : m00(1), m01(0), m02(0),
        m10(0), m11(1), m12(0),
        m20(0), m21(0), m22(1) {
    }

    Matrix3x3(float _m00, float _m01, float _m02,
        float _m10, float _m11, float _m12,
        float _m20, float _m21, float _m22)
        : m00(_m00), m01(_m01), m02(_m02),
        m10(_m10), m11(_m11), m12(_m12),
        m20(_m20), m21(_m21), m22(_m22) {
    }

    //Factorias

    static Matrix3x3 Identity() {
        return Matrix3x3();
    }

    static Matrix3x3 Scale(float sx, float sy, float sz) {
        return Matrix3x3(
            sx, 0, 0,
            0, sy, 0,
            0, 0, sz
        );
    }

    static Matrix3x3 RotationX(float rad) {
        float c = std::cos(rad);
        float s = std::sin(rad);
        return Matrix3x3(
            1, 0, 0,
            0, c, -s,
            0, s, c
        );
    }

    static Matrix3x3 RotationY(float rad) {
        float c = std::cos(rad);
        float s = std::sin(rad);
        return Matrix3x3(
            c, 0, s,
            0, 1, 0,
            -s, 0, c
        );
    }

    static Matrix3x3 RotationZ(float rad) {
        float c = std::cos(rad);
        float s = std::sin(rad);
        return Matrix3x3(
            c, -s, 0,
            s, c, 0,
            0, 0, 1
        );
    }

    //Operadores

    Matrix3x3 operator*(const Matrix3x3& r) const {
        return Matrix3x3(
            m00 * r.m00 + m01 * r.m10 + m02 * r.m20,
            m00 * r.m01 + m01 * r.m11 + m02 * r.m21,
            m00 * r.m02 + m01 * r.m12 + m02 * r.m22,

            m10 * r.m00 + m11 * r.m10 + m12 * r.m20,
            m10 * r.m01 + m11 * r.m11 + m12 * r.m21,
            m10 * r.m02 + m11 * r.m12 + m12 * r.m22,

            m20 * r.m00 + m21 * r.m10 + m22 * r.m20,
            m20 * r.m01 + m21 * r.m11 + m22 * r.m21,
            m20 * r.m02 + m21 * r.m12 + m22 * r.m22
        );
    }

    Vector3D operator*(const Vector3D& v) const {
        return Vector3D(
            m00 * v.Getx() + m01 * v.Gety() + m02 * v.Getz(),
            m10 * v.Getx() + m11 * v.Gety() + m12 * v.Getz(),
            m20 * v.Getx() + m21 * v.Gety() + m22 * v.Getz()
        );
    }

    //Utilidades

    std::string ToString() const {
        std::ostringstream os;
        os << "[[" << m00 << ", " << m01 << ", " << m02 << "], "
           << "[" << m10 << ", " << m11 << ", " << m12 << "], "
           << "[" << m20 << ", " << m21 << ", " << m22 << "]]";
        return os.str();
    }
};
