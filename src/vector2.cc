#include "vector2.h"

myfloat Angle(const Vector2 &vec) {
  if (vec.x() == 0 && vec.y() == 0) return 0;
  myfloat angle = asin(vec.y() / vec.norm());
  if (vec.x() < 0) {
    angle = M_PI - angle;
  }
  return angle;
}

myfloat NormAngle(const Vector2 &vec) {
  if (vec.x() == 0 && vec.y() == 0) return 0;
  myfloat angle = asin(vec.y());
  if (vec.x() < 0) {
    angle = M_PI - angle;
  }
  return angle;
}

myfloat Angle(const Vector2 &a, const Vector2 &b) {
  return Angle(b) - Angle(a);
}

myfloat NormAngle(const Vector2 &a, const Vector2 &b) {
  return NormAngle(b) - NormAngle(a);
}

#ifndef USE_EIGEN

Vector2 &Vector2::operator +=(const Vector2 &other) {
  x() += other.x();
  y() += other.y();
  return *this;
}

Vector2 &Vector2::operator -=(const Vector2 &other) {
  x() -= other.x();
  y() -= other.y();
  return *this;
}

Vector2 &Vector2::operator *=(const myfloat num) {
  x() *= num;
  y() *= num;
  return *this;
}

Vector2 &Vector2::operator /=(const myfloat num) {
  x() /= num;
  y() /= num;
  return *this;
}

Vector2 operator +(const Vector2 &a, const Vector2 &b) {
  Vector2 r(a);
  r += b;
  return r;
}

Vector2 operator -(const Vector2 &a, const Vector2 &b) {
  Vector2 r(a);
  r -= b;
  return r;
}

Vector2 operator *(const Vector2 &point, const myfloat num) {
  Vector2 p(point);
  p *= num;
  return p;
}

Vector2 operator *(const myfloat num, const Vector2 &point) {
  Vector2 p(point);
  p *= num;
  return p;
}

Vector2 operator /(const Vector2 &point, const myfloat num) {
  Vector2 p(point);
  p /= num;
  return p;
}

#endif
