#include "common/math.h"
#include "common/exception.h"
#include "matrix4.h"

#ifndef USE_EIGEN

Matrix34 operator *(const Matrix34 &a, const Matrix34 &b) {
  Matrix34 r;
  myfloat *ri = r.data();
  const myfloat *bstart = b.data();
  for (size_t i = 0; i < Matrix34::kMatrixWidth - 1; ++i) {
    for (size_t j = 0; j < Matrix34::kMatrixHeight; ++j, ++ri) {
      myfloat cr = 0;
      const myfloat *ai = a.data() + j, *bi = bstart;
      for (size_t k = 0; k < Matrix34::kMatrixHeight - 1; ++k) {
        cr += *ai * *bi;
        ai += Matrix34::kMatrixHeight;
        ++bi;
      }
      cr += *ai * *bi;
      *ri = cr;
    }
    bstart += Matrix34::kMatrixHeight;
  }
  for (size_t j = 0; j < Matrix34::kMatrixHeight; ++j, ++ri) {
    myfloat cr = 0;
    const myfloat *ai = a.data() + j, *bi = bstart;
    for (size_t k = 0; k < Matrix34::kMatrixHeight - 1; ++k) {
      cr += *ai * *bi;
      ai += Matrix34::kMatrixHeight;
      ++bi;
    }
    cr += *ai * *bi;
    ai += Matrix34::kMatrixHeight;
    cr += *ai;
    *ri = cr;
  }
  return r;
}

Vector3 operator *(const Matrix34 &matrix, const Vector3 &vector) {
  Vector3 r;
  const myfloat *c = matrix.data();
  r.x() = c[Matrix34::kMatrixHeight * 0 + 0] * vector.x() + c[Matrix34::kMatrixHeight * 1 + 0] * vector.y()
      + c[Matrix34::kMatrixHeight * 2 + 0] * vector.z() + c[Matrix34::kMatrixHeight * 3 + 0];
  r.y() = c[Matrix34::kMatrixHeight * 0 + 1] * vector.x() + c[Matrix34::kMatrixHeight * 1 + 1] * vector.y()
      + c[Matrix34::kMatrixHeight * 2 + 1] * vector.z() + c[Matrix34::kMatrixHeight * 3 + 1];
  r.z() = c[Matrix34::kMatrixHeight * 0 + 2] * vector.x() + c[Matrix34::kMatrixHeight * 1 + 2] * vector.y()
      + c[Matrix34::kMatrixHeight * 2 + 2] * vector.z() + c[Matrix34::kMatrixHeight * 3 + 2];
  return r;
}

static const myfloat kIdentityMatrix34[] = {1,0,0,
                                            0,1,0,
                                            0,0,1,
                                            0,0,0};

RotateTransform::RotateTransform(const myfloat angle, const Vector3UnitX &) : AffineTransform(kIdentityMatrix34) {
  myfloat sina = sin(angle);
  myfloat cosa = cos(angle);
  operator ()(1, 1) = cosa;
  operator ()(1, 2) = -sina;
  operator ()(2, 1) = sina;
  operator ()(2, 2) = cosa;
}

RotateTransform::RotateTransform(const myfloat angle, const Vector3UnitY &) : AffineTransform(kIdentityMatrix34) {
  myfloat sina = sin(angle);
  myfloat cosa = cos(angle);
  operator ()(0, 0) = cosa;
  operator ()(0, 2) = sina;
  operator ()(2, 0) = -sina;
  operator ()(2, 2) = cosa;
}

RotateTransform::RotateTransform(const myfloat angle, const Vector3UnitZ &) : AffineTransform(kIdentityMatrix34) {
  myfloat sina = sin(angle);
  myfloat cosa = cos(angle);
  operator ()(0, 0) = cosa;
  operator ()(0, 1) = -sina;
  operator ()(1, 0) = sina;
  operator ()(1, 1) = cosa;
}

RotateTransform::RotateTransform(const myfloat angle, const Vector3 &a) : AffineTransform(0.0) {
  myfloat cosa = cos(angle);
  myfloat rcosa = 1 - cosa;
  myfloat sina = sin(angle);

  operator ()(0, 0) = cosa + Sqr(a.x()) * rcosa;
  operator ()(1, 0) = a.y() * a.x() * rcosa + a.z() * sina;
  operator ()(2, 0) = a.z() * a.x() * rcosa - a.y() * sina;

  operator ()(0, 1) = a.x() * a.y() * rcosa - a.z() * sina;
  operator ()(1, 1) = cosa + Sqr(a.y()) * rcosa;
  operator ()(2, 1) = a.z() * a.y() * rcosa + a.x() * sina;

  operator ()(0, 2) = a.x() * a.z() * rcosa + a.y() * sina;
  operator ()(1, 2) = a.y() * a.z() * rcosa - a.x() * sina;
  operator ()(2, 2) = cosa + Sqr(a.z()) * rcosa;
}

TranslateTransform::TranslateTransform(const myfloat x, const myfloat y, const myfloat z) : AffineTransform(kIdentityMatrix34) {
  operator ()(0, 3) = x;
  operator ()(1, 3) = y;
  operator ()(2, 3) = z;
}

AffineTransform Scaling(const myfloat kx, const myfloat ky, const myfloat kz) {
  AffineTransform matrix(0.0);
  matrix(0, 0) = kx;
  matrix(1, 1) = ky;
  matrix(2, 2) = kz;
  return matrix;
}

Matrix34 &Matrix34::operator *=(const Matrix34 &other) {
  *this = *this * other;
  return *this;
}


Matrix34::Matrix34() = default;
Matrix34::Matrix34(const myfloat fill) : Matrix<3, 4>(fill) {}

Matrix34::Matrix34(const Matrix4 &matrix4) {
  const myfloat *o = matrix4.data();
  for (myfloat *m = data(); m < data() + kMatrixWidth * kMatrixHeight;
       m += kMatrixHeight, o += Matrix4::kMatrixHeight) {
    memcpy(m, o, kMatrixHeight * sizeof(float));
  }
}

Matrix34::Matrix34(const myfloat *array) : Matrix<3, 4>(array) {}
Matrix34::Matrix34(const Matrix34 &other) : Matrix<3, 4>(other) {}

#endif

void TransformToEuler(const AffineTransform &transform, myfloat &roll, myfloat &pitch, myfloat &yaw) {
  myfloat siny = -transform(2, 0);
  // there goes assumption than cos(y) >= 0:
  if (siny >= 1) throw Exception("Invalid rotation matrix");
  myfloat cosy = sqrt(1 - Sqr(siny));
  myfloat sinx = transform(2, 1) / cosy;
  myfloat cosx = transform(2, 2) / cosy;
  myfloat cosz = transform(0, 0) / cosy;
  myfloat sinz = transform(1, 0) / cosy;
  // now let's check if our assumption was correct
  myfloat check1 = cosx * cosz + sinx * siny * sinz;
  if (fabs(check1 - transform(1, 1)) > 0.1) {
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

void TransformToTranslate(const AffineTransform &transform, myfloat &x, myfloat &y, myfloat &z) {
  x = transform(0, 3);
  y = transform(1, 3);
  z = transform(2, 3);
}
