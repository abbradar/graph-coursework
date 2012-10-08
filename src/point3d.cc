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
  r.x = c[Matrix4::kMatrixWidth * 0 + 0] * point.x + c[Matrix4::kMatrixWidth * 0 + 1] * point.y
      + c[Matrix4::kMatrixWidth * 0 + 2] * point.z + c[Matrix4::kMatrixWidth * 0 + 3] * point.w;
  r.y = c[Matrix4::kMatrixWidth * 1 + 0] * point.x + c[Matrix4::kMatrixWidth * 1 + 1] * point.y
      + c[Matrix4::kMatrixWidth * 1 + 2] * point.z + c[Matrix4::kMatrixWidth * 1 + 3] * point.w;
  r.z = c[Matrix4::kMatrixWidth * 2 + 0] * point.x + c[Matrix4::kMatrixWidth * 2 + 1] * point.y
      + c[Matrix4::kMatrixWidth * 2 + 2] * point.z + c[Matrix4::kMatrixWidth * 2 + 3] * point.w;
  r.w = c[Matrix4::kMatrixWidth * 3 + 0] * point.x + c[Matrix4::kMatrixWidth * 3 + 1] * point.y
      + c[Matrix4::kMatrixWidth * 3 + 2] * point.z + c[Matrix4::kMatrixWidth * 3 + 3] * point.w;
  return r;
}
