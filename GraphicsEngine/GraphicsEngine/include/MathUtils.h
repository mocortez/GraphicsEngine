#pragma once
#include <cmath>
#include <algorithm>

//Utilidades matemáticas generales para la librería. Incluye potencias, raíces, clamp, interpolación y comparaciones seguras.
 
namespace MathUtils
{
    //Constantes

    constexpr float PI = 3.14159265359f;
    constexpr float DEG2RAD = PI / 180.0f;
    constexpr float RAD2DEG = 180.0f / PI;

    //Raices y potencias

    // Potencia entera (más rápida que std::pow)
    inline float Pow(float base, int exp)
    {
        float result = 1.0f;
        for (int i = 0; i < std::abs(exp); ++i)
            result *= base;

        return (exp < 0) ? 1.0f / result : result;
    }

    // Raíz cuadrada
    inline float Sqrt(float value)
    {
        return std::sqrt(value);
    }

	//clamp e interpolación

    inline float Clamp(float value, float min, float max)
    {
        return std::max(min, std::min(value, max));
    }

    inline float Lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

	//Comparaciones flotantes

    inline bool AlmostEqual(float a, float b, float epsilon = 0.00001f)
    {
        return std::fabs(a - b) <= epsilon;
    }

    //Ángulos

    inline float ToRadians(float degrees)
    {
        return degrees * DEG2RAD;
    }

    inline float ToDegrees(float radians)
    {
        return radians * RAD2DEG;
    }
}