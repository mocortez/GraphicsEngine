#pragma once
#include "Prerequisites.h"

/**
 * @brief Representa un vector 2D usado en cálculos típicos de videojuegos,
 * incluyendo física, movimiento, transformaciones y operaciones matemáticas comunes.
 */
class
	Vector2D {
public:
	// CONSTRUCTORES
	/// @brief Constructor por defecto. Inicializa el vector en (0, 0).
	Vector2D() : m_x(0.0f), m_y(0.0f) {}

	/// @brief Constructor principal.
	Vector2D(float x, float y) : m_x(x), m_y(y) {}

	/// @brief Constructor de copia.
	Vector2D(const Vector2D& other) : m_x(other.m_x), m_y(other.m_y) {}

	// Destructor de la clase
	~Vector2D() = default;

	// GETTERS Y SETTERS
	float
		GetX() const {
		return m_x;
	}

	float
		GetY() const {
		return m_y;
	}

	void
		SetX(float value) {
		m_x = value;
	}

	void
		SetY(float value) {
		m_y = value;
	}

	/// @brief Establece ambos valores del vector.
	void
		Set(float newX, float newY) {
		m_x = newX;
		m_y = newY;
	}

	void
		Set(const Vector2D& other) {
		m_x = other.m_x;
		m_y = other.m_y;
	}

	Vector2D
		set() const {
		return Vector2D(m_x, m_y);
	}

	float Magnitude() const {
		return std::sqrt(m_x * m_x + m_y * m_y);
	}

	float SqrMagnitude() const {
		return (m_x * m_x + m_y * m_y);
	}

	void Normalize() {
		float m = Magnitude();
		if (m > 0.0f) {
			m_x /= m;
			m_y /= m;
		}
	}

	Vector2D Normalized() const {
		float m = Magnitude();
		if (m == 0.0f) return Vector2D(0, 0);
		return Vector2D(m_x / m, m_y / m);
	}

	// OPERACIONES VECTORIALES

	/// @brief Producto punto entre este vector y otro.
	float
		Dot(const Vector2D& other) const {
		return (m_x * other.m_x + m_y * other.m_y);
	}

	/// @brief Distancia entre dos vectores.
	float
		Distance(const Vector2D& other) const {
		float dx = m_x - other.m_x;
		float dy = m_y - other.m_y;
		return std::sqrt(dx * dx + dy * dy);
	}

	/// @brief Devuelve un vector perpendicular (rotado 90°).
	Vector2D
		Perpendicular() const {
		return Vector2D(-m_y, m_x);
	}

	/// @brief Limita la magnitud del vector.
	void ClampMagnitude(float maxLength) {
		float m = Magnitude();
		if (m > maxLength) {
			Normalize();
			m_x *= maxLength;
			m_y *= maxLength;
		}
	}

	// SOBRECARGA DE OPERADORES
	Vector2D operator+(const Vector2D& rhs) const {
		return Vector2D(m_x + rhs.m_x, m_y + rhs.m_y);
	}

	Vector2D operator-(const Vector2D& rhs) const {
		return Vector2D(m_x - rhs.m_x, m_y - rhs.m_y);
	}

	Vector2D& operator+=(const Vector2D& rhs) {
		m_x += rhs.m_x;
		m_y += rhs.m_y;
		return *this;
	}

	Vector2D& operator-=(const Vector2D& rhs) {
		m_x -= rhs.m_x;
		m_y -= rhs.m_y;
		return *this;
	}

	Vector2D operator*(float scalar) const {
		return Vector2D(m_x * scalar, m_y * scalar);
	}

	Vector2D operator/(float scalar) const {
		return Vector2D(m_x / scalar, m_y / scalar);
	}

	Vector2D& operator*=(float scalar) {
		m_x *= scalar;
		m_y *= scalar;
		return *this;
	}

	Vector2D& operator/=(float scalar) {
		m_x /= scalar;
		m_y /= scalar;
		return *this;
	}

	/// @brief Operador unario negativo.
	Vector2D operator-() const {
		return Vector2D(-m_x, -m_y);
	}

	/// @brief Comparación por igualdad.
	bool operator==(const Vector2D& rhs) const {
		return (m_x == rhs.m_x && m_y == rhs.m_y);
	}

	bool operator!=(const Vector2D& rhs) const {
		return !(*this == rhs);
	}

	// UTILIDADES
	/// @brief Interpolación lineal (lerp) entre dos vectores.
	static Vector2D Lerp(const Vector2D& a, const Vector2D& b, float t) {
		return Vector2D(
			a.m_x + (b.m_x - a.m_x) * t,
			a.m_y + (b.m_y - a.m_y) * t
		);
	}

	/// @brief Devuelve un string con el formato "(x, y)".
	std::string ToString() const {
		std::ostringstream os;
		os << "(" << m_x << ", " << m_y << ")";
		return os.str();
	}

private:
	float m_x;
	float m_y;
};


