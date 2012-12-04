#include "common/math.h"
#include "common/debug.h"
#include "point2d.h"

Point2D &Point2D::operator +=(const Point2D &other) {
  x += other.x;
  y += other.y;
  return *this;
}

Point2D &Point2D::operator -=(const Point2D &other) {
  x -= other.x;
  y -= other.y;
  return *this;
}

Point2D operator +(const Point2D &a, const Point2D &b) {
  Point2D r(a);
  r += b;
  return r;
}

Point2D operator -(const Point2D &a, const Point2D &b) {
  Point2D r(a);
  r -= b;
  return r;
}

myfloat Point2D::DistanceSqr(const Point2D &a, const Point2D &b) {
  return Sqr(a.x - b.x) + Sqr(a.y - b.y);
}

myfloat Point2D::Distance(const Point2D &a, const Point2D &b) {
  return sqrt(DistanceSqr(a, b));
}

myfloat Point2D::Angle(const Point2D &other) const {
  return other.Angle() - Angle();
}

/** Returns angle; -1/2 * pi <= angle <= 3/2 * pi **/
myfloat Point2D::Angle() const {
  if (x == 0 && y == 0) return 0;
  myfloat angle = asin(y / Length());
  if (x < 0) {
    angle = M_PI - angle;
  }
  return angle;
}

myfloat Point2D::Length() const {
  return sqrt(LengthSqr());
}
myfloat Point2D::LengthSqr() const{
  return Sqr(x) + Sqr(y);
}
