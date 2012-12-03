#ifndef GRAPH_POINT3D_H_
#define GRAPH_POINT3D_H_

struct Point3D;

#include "xparse/xdata.h"
#include "matrix4.h"
#include "myfloat.h"

struct Point3D {
 public:
  myfloat x;
  myfloat y;
  myfloat z;
  myfloat w;

  inline Point3D() : x(0), y(0), z(0), w(0) {}
  inline Point3D(myfloat x_, myfloat y_, myfloat z_, myfloat w_ = 1.0f) :
      x(x_), y(y_), z(z_), w(w_) {}
  inline ~Point3D() = default;

  Point3D &operator +=(const Point3D &other);
  Point3D &operator -=(const Point3D &other);
  Point3D &operator *=(const Matrix4 &matrix);

  template <class T> Point3D &operator *=(const T num) {
    x *= num;
    y *= num;
    z *= num;
    w *= num;
    return *this;
  }

  void WeightNormalize();

  static Point3D VectorMul(const Point3D &a, const Point3D &b);
  static myfloat ScalarMul(const Point3D &a, const Point3D &b);
  static myfloat Distance(const Point3D &a, const Point3D &b);
  static myfloat DistanceSqr(const Point3D &a, const Point3D &b);

 private:
  friend Point3D operator +(const Point3D &a, const Point3D &b);
  friend Point3D operator -(const Point3D &a, const Point3D &b);
  friend Point3D operator *(const Matrix4 &matrix, const Point3D &point);
};

Point3D operator +(const Point3D &a, const Point3D &b);
Point3D operator -(const Point3D &a, const Point3D &b);
Point3D operator *(const Matrix4 &matrix, const Point3D &point);

template <class T> Point3D operator *(const Point3D &point, const T num) {
  Point3D p(point);
  p *= num;
  return p;
}

template <class T> Point3D operator *(const T num, const Point3D &point) {
  return point * num;
}

#endif // GRAPH_POINT3D_H_
