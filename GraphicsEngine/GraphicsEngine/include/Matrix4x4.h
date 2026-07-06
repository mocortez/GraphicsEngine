#pragma once
#include "Prerequisites.h"
#include "Vector4D.h"
#include <cmath> // Para sin, cos, tan

// Alineación estricta a 16 bytes para operaciones óptimas en la GPU (DirectX 11 Constant Buffers)
class alignas(16) Matrix4x4
{
public:
    // Arreglo bidimensional que almacena los coeficientes de la matriz [fila][columna]
    float m[4][4];

    // Constructor por defecto: Inicializa como Matriz Identidad
    Matrix4x4() {
        setIdentity();
    }

    // Configura la matriz con 1s en la diagonal principal y 0s en el resto
    void setIdentity() {
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }

    // --- MÉTODOS DE INSTANCIA (Transformaciones de Mundo) ---

    // Configura una rotación sobre el eje X (Cabeceo / Pitch)
    void setRotationX(float rad) {
        setIdentity();
        m[1][1] = cos(rad);  m[1][2] = -sin(rad);
        m[2][1] = sin(rad);  m[2][2] = cos(rad);
    }

    // Configura una rotación sobre el eje Y (Guiñada / Yaw)
    void setRotationY(float rad) {
        setIdentity();
        m[0][0] = cos(rad);  m[0][2] = sin(rad);
        m[2][0] = -sin(rad); m[2][2] = cos(rad);
    }

    // Configura una rotación sobre el eje Z (Alabeo / Roll)
    void setRotationZ(float rad) {
        setIdentity();
        m[0][0] = cos(rad);  m[0][1] = -sin(rad);
        m[1][0] = sin(rad);  m[1][1] = cos(rad);
    }

    // CORRECCIÓN MATEMÁTICA: Traslación dispuesta para la multiplicación nativa de DirectX
    void setTranslation(float x, float y, float z) {
        setIdentity();
        m[3][0] = x;
        m[3][1] = y;
        m[3][2] = z;
    }

    // Configura el escalado uniforme o por eje
    void setScale(float x, float y, float z) {
        setIdentity();
        m[0][0] = x;
        m[1][1] = y;
        m[2][2] = z;
    }

    // --- MÉTODOS DE CÁMARA Y PERSPECTIVA ---

    // Configura una matriz de proyección en perspectiva (Mano Izquierda / Left-Handed para DirectX)
    void setPerspectiveFovLH(float fov, float aspect, float znear, float zfar) {
        setIdentity();

        float tanHalfFov = tan(fov / 2.0f);

        m[0][0] = 1.0f / (aspect * tanHalfFov);
        m[1][1] = 1.0f / tanHalfFov;
        m[2][2] = zfar / (zfar - znear);
        m[3][2] = (-zfar * znear) / (zfar - znear); // Ajustado para corresponder a la convención de proyección
        m[2][3] = 1.0f;  // Guarda la profundidad Z original en el componente W para el Perspective Divide
        m[3][3] = 0.0f;
    }

    // --- FACTORÍAS ESTÁTICAS ---

    static Matrix4x4 rotationZ(float rad) {
        Matrix4x4 r;
        r.setRotationZ(rad);
        return r;
    }

    // --- OPERACIONES CRÍTICAS ---

    // Transpone la matriz (Intercambia filas por columnas para empaquetado seguro en Constant Buffers de HLSL)
    void transpose() {
        Matrix4x4 out;
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                out.m[i][j] = m[j][i];
        *this = out;
    }

    // Multiplicación de matrices (Permite encadenar Escala * Rotación * Traslación de forma coherente)
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