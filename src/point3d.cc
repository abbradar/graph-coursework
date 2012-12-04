#include "common/math.h"
#include "point3d.h"

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

void Point3D::WeightNormalize() {
  x /= w;
  y /= w;
  z /= w;
  w = 1;
}

Point3D Point3D::VectorMul(const Point3D &a, const Point3D &b) {
  Point3D result;
  result.x = a.y * b.z - a.z * b.y;
  result.y = a.z * b.x - a.x * b.z;
  result.z = a.x * b.y - a.y * b.x;
  return result;
}

myfloat Point3D::ScalarMul(const Point3D &a, const Point3D &b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

myfloat Point3D::Distance(const Point3D &a, const Point3D &b) {
  return sqrt(DistanceSqr(a, b));
}

myfloat Point3D::DistanceSqr(const Point3D &a, const Point3D &b) {
  return Sqr(a.x - b.x) + Sqr(a.y - b.y) + Sqr(a.z - b.z);
}

void Point3D::AxisToEuler(const myfloat angle, myfloat &roll, myfloat &pitch, myfloat &yaw) {
  myfloat cosa = cos(angle);
  myfloat rcosa = 1 - cosa;
  myfloat sina = sin(angle);
  myfloat siny = -z * x * rcosa - y * sina;
  // there goes assumption than cos(y) >= 0:
  if (siny >= 1) return;
  myfloat cosy = sqrt(1 - Sqr(siny));
  myfloat sinx = (z * y * rcosa + x * sina) / cosy;
  myfloat cosx = (cosa + z * z * rcosa) / cosy;
  myfloat cosz = (cosa + x * x * rcosa) / cosy;
  myfloat sinz = (y * x * rcosa + z * sina) / cosy;
  // now let's check if our assumption was correct
  myfloat check1 = cosx * cosz + sinx * siny * sinz;
  myfloat check2 = cosa + y * y * rcosa;
  if (abs(check1 - check2) > 0.1) {
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


Point3D Point3D::Inverse() const {
  return Point3D(-x, -y, -z, w);
}
