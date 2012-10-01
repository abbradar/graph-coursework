#include "point3d.h"

Point3D::Point3D() : Point3D(0, 0, 0, 1) {}

Point3D::Point3D(myfloat x_, myfloat y_, myfloat z_, myfloat w_) :
  x(x_), y(y_), z(z_), w(w_) {}

Point3D &Point3D::operator +=(const Point3D &other) {
  x += other.x;
  y += other.y;
  z += other.z;
  return *this;
}

Point3D &Point3D::operator -=(const Point3D &other) {
  x -= other.x;
  y -= other.y;
  z -= other.z;
  return *this;
}

Point3D &Point3D::operator *=(const Matrix4 &matrix) {
  *this = matrix * *this;
  return *this;
}

void Point3D::Normalize() {
  x /= w;
  y /= w;
  z /= w;
  w = 1;
}

Point3D operator +(const Point3D &a, const Point3D &b) {
  Point3D r(a);
  r += b;
  return r;
}

Point3D operator -(const Point3D &a, const Point3D &b) {
  Point3D r(a);
  r -= b;
  return r;
}

Point3D operator *(const Matrix4 &matrix, const Point3D &point) {
  Point3D r;
  myfloat *c = matrix.matrix_;
  r.x = c[0] * point.x + c[1] * point.y + c[2] * point.z + c[3] * point.w;
  r.y = c[4] * point.x + c[5] * point.y + c[6] * point.z + c[7] * point.w;
  r.z = c[8] * point.x + c[9] * point.y + c[10] * point.z + c[11] * point.w;
  r.w = c[12] * point.x + c[13] * point.y + c[14] * point.z + c[15] * point.w;
  return r;
}
