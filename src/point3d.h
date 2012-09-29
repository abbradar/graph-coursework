#ifndef GRAPH_POINT3D_H_
#define GRAPH_POINT3D_H_

struct Point3D;

#include "matrix4.h"

struct Point3D {
 public:
  float x;
  float y;
  float z;
  float w;

  Point3D();
  Point3D(float x, float y, float z);

  Point3D &operator +=(const Point3D &other);
  Point3D &operator -=(const Point3D &other);
  Point3D &operator *=(const Matrix4 &matrix);

  void Normalize();

 private:
  friend Point3D operator +(const Point3D &a, const Point3D &b);
  friend Point3D operator -(const Point3D &a, const Point3D &b);
  friend Point3D operator *(const Matrix4 &matrix, const Point3D &point);
};

Point3D operator +(const Point3D &a, const Point3D &b);
Point3D operator -(const Point3D &a, const Point3D &b);
Point3D operator *(const Matrix4 &matrix, const Point3D &point);

#endif // GRAPH_POINT3D_H_
