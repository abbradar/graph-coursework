#include "point2d.h"

Point2D::Point2D() : Point2D(0, 0) {}

Point2D::Point2D(myfloat x_, myfloat y_) : x(x_), y(y_) {}

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
