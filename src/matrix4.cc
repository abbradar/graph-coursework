#include <cstring>
#include <cmath>
#include "common/exception.h"
#include "common/debug.h"
#include "matrix4.h"

using namespace xparse;

const size_t Matrix4::kMatrixWidth = 4;
const size_t Matrix4::kMatrixHeight = 4;

Matrix4::Matrix4() : Matrix4(0.0) {}

Matrix4::Matrix4(myfloat fill) : matrix_(new myfloat[kMatrixWidth * kMatrixHeight]) {
  for (size_t i = 0; i < kMatrixWidth * kMatrixHeight; ++i) {
    matrix_[i] = fill;
  }
}

Matrix4::Matrix4(const myfloat *array): matrix_(new myfloat[kMatrixWidth * kMatrixHeight]) {
  memcpy(matrix_, array, kMatrixWidth * kMatrixHeight * sizeof(myfloat));
}

Matrix4::Matrix4(const Matrix4 &other) : matrix_(new myfloat[kMatrixWidth * kMatrixHeight]) {
  operator =(other);
}

Matrix4::~Matrix4() {
  delete[] matrix_;
}

Matrix4 &Matrix4::operator =(const Matrix4 &other) {
  memcpy(matrix_, other.matrix_, kMatrixWidth * kMatrixHeight * sizeof(myfloat));
  return *this;
}

Matrix4 Matrix4::Translate(myfloat x, myfloat y, myfloat z) {
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

Matrix4 Matrix4::RotateX(myfloat a) {
  Matrix4 matrix;
  myfloat sina = sin(a);
  myfloat cosa = cos(a);
  matrix.at(0, 0) = 1;
  matrix.at(3, 3) = 1;
  matrix.at(1, 1) = cosa;
  matrix.at(2, 1) = -sina;
  matrix.at(1, 2) = sina;
  matrix.at(2, 2) = cosa;
  return matrix;
}

Matrix4 Matrix4::RotateY(myfloat a) {
  Matrix4 matrix;
  myfloat sina = sin(a);
  myfloat cosa = cos(a);
  matrix.at(1, 1) = 1;
  matrix.at(3, 3) = 1;
  matrix.at(0, 0) = cosa;
  matrix.at(2, 0) = -sina;
  matrix.at(0, 2) = sina;
  matrix.at(2, 2) = cosa;
  return matrix;
}

Matrix4 Matrix4::RotateZ(myfloat a) {
  Matrix4 matrix;
  myfloat sina = sin(a);
  myfloat cosa = cos(a);
  matrix.at(2, 2) = 1;
  matrix.at(3, 3) = 1;
  matrix.at(0, 0) = cosa;
  matrix.at(1, 0) = -sina;
  matrix.at(0, 1) = sina;
  matrix.at(1, 1) = cosa;
  return matrix;
}

Matrix4 Matrix4::Scale(myfloat x, myfloat y, myfloat z) {
  Matrix4 matrix;
  matrix.at(3, 3) = 1;
  matrix.at(0, 0) = x;
  matrix.at(1, 1) = y;
  matrix.at(2, 2) = z;
  return matrix;
}

Matrix4 Matrix4::Identity() {
  static const myfloat kIdentityMatrix[] = {1,0,0,0,
                                            0,1,0,0,
                                            0,0,1,0,
                                            0,0,0,1};
  return Matrix4(kIdentityMatrix);
}

Matrix4 Matrix4::LoadFromXTransformMatrix(xparse::XData *data) {
  if (data->template_id != "FrameTransformMatrix") {
    throw Exception("FrameTransformMatrix type expected.");
  }
  Matrix4 matrix;
  XDataValue *matrix_node = (*data->data[0]->data().node_value)[0].get();
  auto array = matrix_node->data().array_value;
  Assert(array->size() == kMatrixWidth * kMatrixHeight);
  for (size_t i = 0; i < kMatrixWidth * kMatrixHeight; ++i) {
    matrix.data()[i] = (*array)[i].float_value;
  }
  return matrix;
}

Matrix4 &Matrix4::operator +=(const Matrix4 &other) {
  myfloat *ti = matrix_, *oi = other.matrix_;
  for (size_t i = 0; i < Matrix4::kMatrixHeight * Matrix4::kMatrixWidth; ++ti, ++oi, ++i)
    *ti += *oi;
  return *this;
}

Matrix4 &Matrix4::operator -=(const Matrix4 &other) {
  myfloat *ti = matrix_, *oi = other.matrix_;
  for (size_t i = 0; i < Matrix4::kMatrixHeight * Matrix4::kMatrixWidth; ++ti, ++oi, ++i)
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
  myfloat *ri = r.matrix_;
  myfloat *astart = a.matrix_;
  for (size_t i = 0; i < Matrix4::kMatrixHeight; ++i) {
    for (size_t j = 0; j < Matrix4::kMatrixWidth; ++j, ++ri) {
      myfloat cr = 0;
      myfloat *ai = astart, *bi = b.matrix_ + j;
      for (size_t k = 0; k < Matrix4::kMatrixHeight; ++k) {
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
