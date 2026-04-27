#pragma once
#include "Prerequisites.h"
#include "Vector4D.h"
#include <cmath> // Para sin, cos, tan

class alignas(16) Matrix4x4
{
public:
    float m[4][4];

    Matrix4x4() {
        setIdentity();
    }

    void setIdentity() {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    // --- MÉTODOS DE INSTANCIA (Para cb.m_world.setRotation...) ---

    void setRotationX(float rad) {
        setIdentity();
        m[1][1] = cos(rad);  m[1][2] = -sin(rad);
        m[2][1] = sin(rad);  m[2][2] = cos(rad);
    }

    void setRotationY(float rad) {
        setIdentity();
        m[0][0] = cos(rad);  m[0][2] = sin(rad);
        m[2][0] = -sin(rad); m[2][2] = cos(rad);
    }

    void setRotationZ(float rad) {
        setIdentity();
        m[0][0] = cos(rad);  m[0][1] = -sin(rad);
        m[1][0] = sin(rad);  m[1][1] = cos(rad);
    }

    void setTranslation(float x, float y, float z) {
        setIdentity();
        m[0][3] = x;
        m[1][3] = y;
        m[2][3] = z;
    }

    void setScale(float x, float y, float z) {
        setIdentity();
        m[0][0] = x;
        m[1][1] = y;
        m[2][2] = z;
    }

    // --- FACTORÍAS ESTÁTICAS (Tu estilo original) ---

    static Matrix4x4 rotationZ(float rad) {
        Matrix4x4 r;
        r.setRotationZ(rad);
        return r;
    }

    // --- OPERACIONES CRÍTICAS ---

    void transpose() {
        Matrix4x4 out;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                out.m[i][j] = m[j][i];
        *this = out;
    }

    Matrix4x4 operator*(const Matrix4x4& rhs) const {
        Matrix4x4 r;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                float sum = 0;
                for (int k = 0; k < 4; ++k)
                    sum += m[i][k] * rhs.m[k][j];
                r.m[i][j] = sum;
            }
        }
        return r;
    }
};