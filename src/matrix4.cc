#include <cstring>
#include <cmath>
#include "matrix4.h"

const int Matrix4::kMatrixWidth = 4;
const int Matrix4::kMatrixHeight = 4;

Matrix4::Matrix4() : Matrix4(0.0) {}

Matrix4::Matrix4(my_float fill) : matrix_(new my_float[kMatrixWidth * kMatrixHeight]) {
  for (int i = 0; i < kMatrixWidth * kMatrixHeight; ++i) {
    matrix_[i] = fill;
  }
}

Matrix4::Matrix4(const my_float *array): matrix_(new my_float[kMatrixWidth * kMatrixHeight]) {
  memcpy(matrix_, array, kMatrixWidth * kMatrixHeight * sizeof(my_float));
}

Matrix4::Matrix4(const Matrix4 &other) : matrix_(new my_float[kMatrixWidth * kMatrixHeight]) {
  operator =(other);
}

Matrix4::~Matrix4() {
  delete matrix_;
}

Matrix4 &Matrix4::operator =(const Matrix4 &other) {
  memcpy(matrix_, other.matrix_, kMatrixWidth * kMatrixHeight * sizeof(my_float));
  return *this;
}

Matrix4 Matrix4::Translate(my_float x, my_float y, my_float z) {
  Matrix4 matrix;
  matrix.at(0, 0) = 1;
  matrix.at(1, 1) = 1;
  matrix.at(2, 2) = 1;
  matrix.at(3, 3) = 1;
  matrix.at(3, 0) = x;
  matrix.at(3, 1) = y;
  matrix.at(3, 2) = z;
  return matrix;
}

Matrix4 Matrix4::RotateX(my_float a) {
  Matrix4 matrix;
  my_float sina = sin(a);
  my_float cosa = cos(a);
  matrix.at(0, 0) = 1;
  matrix.at(3, 3) = 1;
  matrix.at(1, 1) = cosa;
  matrix.at(2, 1) = -sina;
  matrix.at(1, 2) = sina;
  matrix.at(2, 2) = cosa;
  return matrix;
}

Matrix4 Matrix4::RotateY(my_float a) {
  Matrix4 matrix;
  my_float sina = sin(a);
  my_float cosa = cos(a);
  matrix.at(1, 1) = 1;
  matrix.at(3, 3) = 1;
  matrix.at(0, 0) = cosa;
  matrix.at(2, 0) = sina;
  matrix.at(0, 2) = -sina;
  matrix.at(2, 2) = cosa;
  return matrix;
}

Matrix4 Matrix4::RotateZ(my_float a) {
  Matrix4 matrix;
  my_float sina = sin(a);
  my_float cosa = cos(a);
  matrix.at(2, 2) = 1;
  matrix.at(3, 3) = 1;
  matrix.at(0, 0) = cosa;
  matrix.at(1, 0) = -sina;
  matrix.at(0, 1) = sina;
  matrix.at(1, 1) = cosa;
  return matrix;
}

Matrix4 Matrix4::Scale(my_float x, my_float y, my_float z) {
  Matrix4 matrix;
  matrix.at(3, 3) = 1;
  matrix.at(0, 0) = x;
  matrix.at(1, 1) = y;
  matrix.at(2, 2) = z;
  return matrix;
}

Matrix4 Matrix4::Perspective(my_float ex, my_float ey, my_float ez) {
  Matrix4 matrix;
  matrix.at(0, 0) = 1;
  matrix.at(1, 1) = 1;
  matrix.at(2, 2) = 1;
  matrix.at(3, 0) = -ex;
  matrix.at(3, 1) = -ey;
  matrix.at(2, 3) = 1.0 / ez;
  return matrix;
}

Matrix4 &Matrix4::operator +=(const Matrix4 &other) {
  my_float *ti = matrix_, *oi = other.matrix_;
  for (int i = 0; i < Matrix4::kMatrixHeight * Matrix4::kMatrixWidth; ++ti, ++oi, ++i)
    *ti += *oi;
  return *this;
}

Matrix4 &Matrix4::operator -=(const Matrix4 &other) {
  my_float *ti = matrix_, *oi = other.matrix_;
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
  my_float *ri = r.matrix_;
  my_float *astart = a.matrix_;
  for (int i = 0; i < Matrix4::kMatrixHeight; ++i) {
    for (int j = 0; j < Matrix4::kMatrixWidth; ++j, ++ri) {
      my_float cr = 0;
      my_float *ai = astart, *bi = b.matrix_ + j;
      for (int k = 0; k < Matrix4::kMatrixHeight; ++k) {
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
