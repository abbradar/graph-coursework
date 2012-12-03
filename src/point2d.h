#ifndef GRAPH_POINT2D_H_
#define GRAPH_POINT2D_H_

#include "xparse/xdata.h"
#include "myfloat.h"
#include "point3d.h"

struct Point2D {
 public:
  myfloat x;
  myfloat y;

  inline Point2D() : x(0), y(0) {}
  inline Point2D(myfloat x_, myfloat y_) : x(x_), y(y_) {}
  inline Point2D(const Point3D &p3d) : x(p3d.x), y(p3d.y) {}

  Point2D &operator +=(const Point2D &other);
  Point2D &operator -=(const Point2D &other);

  template <class T> Point2D &operator *(const T num) {
    x *= num;
    y *= num;
    return *this;
  }

  myfloat Angle(const Point2D &other) const;
  myfloat Angle() const;
  myfloat Length() const;
  myfloat LengthSqr() const;

  static myfloat Distance(const Point2D &a, const Point2D &b);
  static myfloat DistanceSqr(const Point2D &a, const Point2D &b);

 private:
  friend Point2D operator +(const Point2D &a, const Point2D &b);
  friend Point2D operator -(const Point2D &a, const Point2D &b);
};

Point2D operator +(const Point2D &a, const Point2D &b);
Point2D operator -(const Point2D &a, const Point2D &b);

template <class T> Point2D operator *(const Point2D &point, const T num) {
  Point2D p(point);
  p *= num;
  return p;
}

template <class T> Point2D operator *(const T num, const Point2D &point) {
  return point * num;
}

#endif // GRAPH_POINT2D_H_
