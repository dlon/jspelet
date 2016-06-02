#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>

template <class T>
struct Vector2D {
	T &operator [](unsigned i) {
		return (i ? y : x);
	}
	Vector2D &operator +=(const Vector2D &i) {
		x += i.x;
		y += i.y;
		return *this;
	}
	Vector2D &operator +=(T i) {
		x += i;
		y += i;
		return *this;
	}
	Vector2D &operator -=(const Vector2D &i) {
		x -= i.x;
		y -= i.y;
		return *this;
	}
	Vector2D &operator -=(T i) {
		x -= i;
		y -= i;
		return *this;
	}
	Vector2D &operator *=(const Vector2D &i) {
		x *= i.x;
		y *= i.y;
		return *this;
	}
	Vector2D &operator *=(T i) {
		x *= i;
		y *= i;
		return *this;
	}
	Vector2D &operator =(T i) {
		x = i;
		y = i;
		return *this;
	}
	Vector2D(T _x, T _y) : x(_x), y(_y) {}
	Vector2D() : x(0), y(0) {}

	Vector2D &Rotate(float rad) {
		float s = std::sin(rad);
		float c = std::cos(rad);
		float _x = x*c-y*s;
		float _y = x*s+y*c;
		x = T(_x);
		y = T(_y);
		return *this;
	}

	T x;
	T y;
};

typedef Vector2D<int> Vector2Di;
typedef Vector2D<float> Vector2Df;
typedef Vector2D<int> Vector2i;
typedef Vector2D<float> Vector2f;
// Vector2i
// Vector2f

#endif // VECTOR2D_H
