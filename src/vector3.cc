#include "common/math.h"
#include "vector3.h"

#ifndef USE_EIGEN

Vector3 &Vector3::operator +=(const Vector3 &other) {
  x() += other.x();
  y() += other.y();
  z() += other.z();
  return *this;
}

Vector3 &Vector3::operator -=(const Vector3 &other) {
  x() -= other.x();
  y() -= other.y();
  z() -= other.z();
  return *this;
}

Vector3 &Vector3::operator *=(const myfloat num) {
  x() *= num;
  y() *= num;
  z() *= num;
  return *this;
}

Vector3 &Vector3::operator /=(const myfloat num) {
  x() /= num;
  y() /= num;
  z() /= num;
  return *this;
}

Vector3 Vector3::cross(const Vector3 &b) const {
  Vector3 result;
  result.x() = y() * b.z() - z() * b.y();
  result.y() = z() * b.x() - x() * b.z();
  result.z() = x() * b.y() - y() * b.x();
  return result;
}

myfloat Vector3::dot(const Vector3 &b) const {
  return x() * b.x() + y() * b.y() + z() * b.z();
}

const Vector3UnitX &Vector3::UnitX() {
  static const Vector3UnitX unit;
  return unit;
}

const Vector3UnitY &Vector3::UnitY() {
  static const Vector3UnitY unit;
  return unit;
}

const Vector3UnitZ &Vector3::UnitZ() {
  static const Vector3UnitZ unit;
  return unit;
}

Vector3 operator +(const Vector3 &a, const Vector3 &b) {
  Vector3 r(a);
  r += b;
  return r;
}

Vector3 operator -(const Vector3 &a, const Vector3 &b) {
  Vector3 r(a);
  r -= b;
  return r;
}

Vector3 operator *(const Vector3 &point, const myfloat num) {
  Vector3 p(point);
  p *= num;
  return p;
}

Vector3 operator *(const myfloat num, const Vector3 &point) {
  Vector3 p(point);
  p *= num;
  return p;
}

Vector3 operator /(const Vector3 &point, const myfloat num) {
  Vector3 p(point);
  p /= num;
  return p;
}

#endif

void AxisToEuler(const Vector3 &p, const myfloat angle, myfloat &roll, myfloat &pitch, myfloat &yaw) {
  myfloat cosa = cos(angle);
  myfloat rcosa = 1 - cosa;
  myfloat sina = sin(angle);
  myfloat siny = -p.z() * p.x() * rcosa + p.y() * sina;
  // there goes assumption than cos(y) >= 0:
  if (siny >= 1) return;
  myfloat cosy = sqrt(1 - Sqr(siny));

  myfloat sinx = (p.z() * p.y() * rcosa + p.x() * sina) / cosy;
  myfloat cosx = (cosa + Sqr(p.z()) * rcosa) / cosy;
  myfloat cosz = (cosa + Sqr(p.x()) * rcosa) / cosy;
  myfloat sinz = (p.y() * p.x() * rcosa + p.z() * sina) / cosy;
  // now let's check if our assumption was correct
  myfloat check1 = cosx * cosz + sinx * siny * sinz;
  myfloat check2 = cosa + Sqr(p.y()) * rcosa;
  if (fabs(check1 - check2) > 0.001) {
    cosy = -cosy;
    sinx = -sinx;
    cosx = -cosx;
    cosz = -cosz;
    sinz = -sinz;
  }
  roll = Angle(sinx, cosx);
  pitch = Angle(siny, cosy);
  yaw = Angle(sinz, cosz);
}
