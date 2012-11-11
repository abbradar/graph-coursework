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

  Point3D();
  Point3D(myfloat x, myfloat y, myfloat z, myfloat w = 1.0f);

  Point3D &operator +=(const Point3D &other);
  Point3D &operator -=(const Point3D &other);
  Point3D &operator *=(const Matrix4 &matrix);

  void WeightNormalize();

  static Point3D VectorMul(const Point3D &a, const Point3D &b);
  static myfloat ScalarMul(const Point3D &a, const Point3D &b);

 private:
  friend Point3D operator +(const Point3D &a, const Point3D &b);
  friend Point3D operator -(const Point3D &a, const Point3D &b);
  friend Point3D operator *(const Matrix4 &matrix, const Point3D &point);
};

Point3D operator +(const Point3D &a, const Point3D &b);
Point3D operator -(const Point3D &a, const Point3D &b);
Point3D operator *(const Matrix4 &matrix, const Point3D &point);

#endif // GRAPH_POINT3D_H_
