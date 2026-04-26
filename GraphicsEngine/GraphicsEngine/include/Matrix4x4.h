#pragma once
#include "Prerequisites.h"
#include "Vector4D.h"
#include <memory> // Para memset

/**
 * @brief Matriz 4x4 optimizada para DirectX 11.
 * Alineada a 16 bytes para compatibilidad con Constant Buffers.
 */
 class alignas(16) Matrix4x4
{
public:
    // Array plano para asegurar que no haya padding entre filas
    float m[4][4];

    // Constructor: Inicializa en Ceros por seguridad
    Matrix4x4() {
        SetIdentity();
    }

    // Establece la identidad de forma explícita
    void SetIdentity() {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    // Factoría: Traslación
    static Matrix4x4 Translation(float x, float y, float z) {
        Matrix4x4 r; // Ya es identidad
        r.m[0][3] = x;
        r.m[1][3] = y;
        r.m[2][3] = z;
        return r;
    }

    // Factoría: Escala
    static Matrix4x4 Scale(float sx, float sy, float sz) {
        Matrix4x4 r; // Ya es identidad
        r.m[0][0] = sx;
        r.m[1][1] = sy;
        r.m[2][2] = sz;
        return r;
    }

    // Factoría: Rotación en Z
    static Matrix4x4 RotationZ(float rad) {
        Matrix4x4 r;
        float c = std::cos(rad);
        float s = std::sin(rad);
        r.m[0][0] = c; r.m[0][1] = -s;
        r.m[1][0] = s; r.m[1][1] = c;
        return r;
    }

    // --- CRÍTICO PARA DIRECTX 11 ---
    // Transpone la matriz para convertir de Row-Major (C++) a Column-Major (HLSL)
    void Transpose() {
        Matrix4x4 out;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                out.m[i][j] = m[j][i];
            }
        }
        *this = out;
    }

    // Multiplicación de Matrices
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

    // Proyección Perspectiva (Ajustada para Left-Handed, estándar de DirectX)
    static Matrix4x4 Perspective(float fovRad, float aspect, float znear, float zfar) {
        Matrix4x4 r;
        // Limpiamos la identidad para poner valores de proyección
        for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) r.m[i][j] = 0.0f;

        float tanHalfFov = std::tan(fovRad / 2.0f);

        r.m[0][0] = 1.0f / (aspect * tanHalfFov);
        r.m[1][1] = 1.0f / (tanHalfFov);
        r.m[2][2] = zfar / (zfar - znear);
        r.m[2][3] = (-zfar * znear) / (zfar - znear);
        r.m[3][2] = 1.0f; // Importante para la división W posterior
        r.m[3][3] = 0.0f;

        return r;
    }

    Vector4D operator*(const Vector4D& v) const {
        return Vector4D(
            m[0][0] * v.Getx() + m[0][1] * v.Gety() + m[0][2] * v.Getz() + m[0][3] * v.Getw(),
            m[1][0] * v.Getx() + m[1][1] * v.Gety() + m[1][2] * v.Getz() + m[1][3] * v.Getw(),
            m[2][0] * v.Getx() + m[2][1] * v.Gety() + m[2][2] * v.Getz() + m[2][3] * v.Getw(),
            m[3][0] * v.Getx() + m[3][1] * v.Gety() + m[3][2] * v.Getz() + m[3][3] * v.Getw()
        );
    }
};