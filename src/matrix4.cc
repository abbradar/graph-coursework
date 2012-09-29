#include <cstring>
#include <cmath>
#include "matrix4.h"

const int Matrix4::kMatrixWidth = 4;
const int Matrix4::kMatrixHeight = 4;

Matrix4::Matrix4() : Matrix4(0.0f) {}

Matrix4::Matrix4(float fill) : matrix_(new float[kMatrixWidth * kMatrixHeight]) {
  for (int i = 0; i < kMatrixWidth * kMatrixHeight; ++i) {
    matrix_[i] = fill;
  }
}

Matrix4::Matrix4(const Matrix4 &other) : matrix_(new float[kMatrixWidth * kMatrixHeight]) {
  operator =(other);
}

Matrix4::~Matrix4() {
  delete matrix_;
}

Matrix4 &Matrix4::operator =(const Matrix4 &other) {
  memcpy(matrix_, other.matrix_, kMatrixWidth * kMatrixHeight * sizeof(float));
  return *this;
}

Matrix4 Matrix4::Translate(float x, float y, float z) {
  Matrix4 matrix;
  matrix.at(0, 0) = 1.0f;
  matrix.at(1, 1) = 1.0f;
  matrix.at(2, 2) = 1.0f;
  matrix.at(3, 3) = 1.0f;
  matrix.at(3, 0) = x;
  matrix.at(3, 1) = y;
  matrix.at(3, 2) = z;
  return matrix;
}

Matrix4 Matrix4::RotateX(float a) {
  Matrix4 matrix;
  float sina = sin(a);
  float cosa = cos(a);
  matrix.at(0, 0) = 1.0f;
  matrix.at(3, 3) = 1.0f;
  matrix.at(1, 1) = cosa;
  matrix.at(2, 1) = -sina;
  matrix.at(1, 2) = sina;
  matrix.at(2, 2) = cosa;
  return matrix;
}

Matrix4 Matrix4::RotateY(float a) {
  Matrix4 matrix;
  float sina = sin(a);
  float cosa = cos(a);
  matrix.at(1, 1) = 1.0f;
  matrix.at(3, 3) = 1.0f;
  matrix.at(0, 0) = cosa;
  matrix.at(2, 0) = sina;
  matrix.at(0, 2) = -sina;
  matrix.at(2, 2) = cosa;
  return matrix;
}

Matrix4 Matrix4::RotateZ(float a) {
  Matrix4 matrix;
  float sina = sin(a);
  float cosa = cos(a);
  matrix.at(2, 2) = 1.0f;
  matrix.at(3, 3) = 1.0f;
  matrix.at(0, 0) = cosa;
  matrix.at(1, 0) = -sina;
  matrix.at(0, 1) = sina;
  matrix.at(1, 1) = cosa;
  return matrix;
}

Matrix4 Matrix4::Scale(float x, float y, float z) {
  Matrix4 matrix;
  matrix.at(3, 3) = 1.0f;
  matrix.at(0, 0) = x;
  matrix.at(1, 1) = y;
  matrix.at(2, 2) = z;
  return matrix;
}

Matrix4 Matrix4::Perspective(float n, float f) {
  Matrix4 matrix;
  matrix.at(0, 0) = 1.0f;
  matrix.at(1, 1) = 1.0f;
  matrix.at(2, 2) = (n + f) / n;
  matrix.at(3, 2) = -f;
  matrix.at(2, 3) = 1 / n;
  return matrix;
}

Matrix4 &Matrix4::operator +=(const Matrix4 &other) {
  float *ti = matrix_, *oi = other.matrix_;
  for (int i = 0; i < Matrix4::kMatrixHeight * Matrix4::kMatrixWidth; ++ti, ++oi, ++i)
    *ti += *oi;
  return *this;
}

Matrix4 &Matrix4::operator -=(const Matrix4 &other) {
  float *ti = matrix_, *oi = other.matrix_;
  for (int i = 0; i < Matrix4::kMatrixHeight * Matrix4::kMatrixWidth; ++ti, ++oi, ++i)
    *ti -= *oi;
  return *this;
}

Matrix4 &Matrix4::operator *=(const Matrix4 &other) {
  *this = *this * other;
  return *this;
}

Matrix4 operator +(const Matrix4 &a, const Matrix4 &b) {
  Matrix4 r(a);
  r += b;
  return r;
}

Matrix4 operator -(const Matrix4 &a, const Matrix4 &b) {
  Matrix4 r(a);
  r -= b;
  return r;
}

Matrix4 operator *(const Matrix4 &a, const Matrix4 &b) {
  Matrix4 r;
  float *ri = r.matrix_;
  float *astart = a.matrix_;
  for (int i = 0; i < Matrix4::kMatrixHeight; ++i) {
    for (int j = 0; j < Matrix4::kMatrixWidth; ++j, ++ri) {
      float cr = 0;
      float *ai = astart, *bi = b.matrix_ + j;
      for (int k = 0; i < Matrix4::kMatrixHeight; ++k) {
        cr += *ai * *bi;
        ++ai;
        bi += Matrix4::kMatrixWidth;
      }
      *ri = cr;
    }
    astart += Matrix4::kMatrixWidth;
  }
  return r;
}
