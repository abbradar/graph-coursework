#ifndef GRAPH_MATRIX4_H_
#define GRAPH_MATRIX4_H_

#include "myfloat.h"
#include "config.h"

#ifdef USE_EIGEN

#include <Eigen/Core>
#include <Eigen/Geometry>

typedef Eigen::Transform<myfloat, 3, Eigen::AffineCompact> AffineTransform;
typedef Eigen::AngleAxis<myfloat> RotateTransform;
typedef Eigen::Translation<myfloat, 3> TranslateTransform;
typedef Eigen::Matrix<myfloat, 4, 4> Matrix4;
using Eigen::Scaling;

#else

#include <cstring>
#include <algorithm>
#include "common/debug.h"
#if DEBUG_LEVEL >= 4
#include "common/exception.h"
#endif

class Matrix34;
typedef Matrix34 AffineTransform;

#include "vector3.h"

template <size_t kTMatrixHeight, size_t kTMatrixWidth> class Matrix {
 public:
  static constexpr size_t kMatrixHeight = kTMatrixHeight;
  static constexpr size_t kMatrixWidth = kTMatrixWidth;

  Matrix() = default;

  explicit Matrix(const myfloat fill) {
    std::fill_n(matrix_, kMatrixWidth * kMatrixHeight, fill);
  }

  Matrix(const myfloat *array) {
    memcpy(matrix_, array, kMatrixWidth * kMatrixHeight * sizeof(myfloat));
  }

  Matrix(const Matrix &other) : Matrix(other.matrix_) {}

  Matrix &operator =(const Matrix &other) {
    memcpy(matrix_, other.matrix_, kMatrixWidth * kMatrixHeight * sizeof(myfloat));
    return *this;
  }

  inline myfloat &operator ()(const size_t y, const size_t x) {
#if DEBUG_LEVEL >= 4
    if (x > kMatrixWidth || y > kMatrixHeight) {
      throw Exception("Matrix index is out of bounds.");
    }
#endif
    return matrix_[x * kMatrixHeight + y];
  }

  inline const myfloat &operator ()(const size_t y, const size_t x) const {
#if DEBUG_LEVEL >= 4
    if (x > kMatrixWidth || y > kMatrixHeight) {
      throw Exception("Matrix index is out of bounds.");
    }
#endif
    return matrix_[x * kMatrixHeight + y];
  }

  inline myfloat *data() {
    return matrix_;
  }

  inline const myfloat *data() const {
    return matrix_;
  }

  static constexpr inline size_t rows() {
    return kMatrixHeight;
  }

  static constexpr inline size_t cols() {
    return kMatrixHeight;
  }

  void transposeInPlace() {
    for (size_t i = 1; i < kMatrixHeight; ++i) {
      for (size_t j = 0; j < i; ++j) {
        std::swap(operator ()(i, j), operator ()(j, i));
      }
    }
  }

  Matrix &operator +=(const Matrix &other) {
    for (myfloat *ti = matrix_, *oi = other.matrix_; ti < matrix_ + kMatrixHeight * kMatrixWidth; ++ti, ++oi)
      *ti += *oi;
    return *this;
  }

  Matrix &operator -=(const Matrix &other) {
    for (myfloat *ti = matrix_, *oi = other.matrix_; ti < matrix_ + kMatrixHeight * kMatrixWidth; ++ti, ++oi)
      *ti -= *oi;
    return *this;
  }

  template <size_t kOMatrixWidth>
    Matrix<kMatrixHeight, kOMatrixWidth> &operator *=(const Matrix<kTMatrixWidth, kOMatrixWidth> &other) {
      *this = *this * other;
      return *this;
    }

 private:
  myfloat matrix_[kMatrixHeight * kMatrixWidth];

  template <size_t kMatrixHeight, size_t kMatrixWidth>
    friend Matrix<kMatrixHeight, kMatrixWidth> operator +(const Matrix<kMatrixHeight, kMatrixWidth> &a,
                                                          const Matrix<kMatrixHeight, kMatrixWidth> &b);
  template <size_t kMatrixHeight, size_t kMatrixWidth>
    friend Matrix<kMatrixHeight, kMatrixWidth> operator -(const Matrix<kMatrixHeight, kMatrixWidth> &a,
                                                          const Matrix<kMatrixHeight, kMatrixWidth> &b);
  template <size_t kMatrixHeight, size_t kMatrixCSize, size_t kMatrixWidth>
    friend Matrix<kMatrixHeight, kMatrixWidth> operator *(const Matrix<kMatrixHeight, kMatrixCSize> &a,
                                                          const Matrix<kMatrixCSize, kMatrixWidth> &b);
};

template <size_t kMatrixHeight, size_t kMatrixWidth>
  Matrix<kMatrixHeight, kMatrixWidth> operator +(const Matrix<kMatrixHeight, kMatrixWidth> &a,
                                                        const Matrix<kMatrixHeight, kMatrixWidth> &b) {
  Matrix<kMatrixHeight, kMatrixWidth> r(a);
  r += b;
  return r;
}

template <size_t kMatrixHeight, size_t kMatrixWidth>
  Matrix<kMatrixHeight, kMatrixWidth> operator -(const Matrix<kMatrixHeight, kMatrixWidth> &a,
                                                        const Matrix<kMatrixHeight, kMatrixWidth> &b) {
  Matrix<kMatrixHeight, kMatrixWidth> r(a);
  r -= b;
  return r;
}

template <size_t kMatrixHeight, size_t kMatrixCSize, size_t kMatrixWidth>
  Matrix<kMatrixHeight, kMatrixWidth> operator *(const Matrix<kMatrixHeight, kMatrixCSize> &a,
                                                        const Matrix<kMatrixCSize, kMatrixWidth> &b) {
  Matrix<kMatrixHeight, kMatrixWidth> r;
  myfloat *ri = r.data();
  const myfloat *bstart = b.data();
  for (size_t i = 0; i < kMatrixWidth; ++i) {
    for (size_t j = 0; j < kMatrixHeight; ++j, ++ri) {
      myfloat cr = 0;
      const myfloat *ai = a.data() + j, *bi = bstart;
      for (size_t k = 0; k < kMatrixCSize - 1; ++k) {
        cr += *ai * *bi;
        ai += kMatrixHeight;
        ++bi;
      }
      cr += *ai * *bi;
      *ri = cr;
    }
    bstart += kMatrixCSize;
  }
  return r;
}

typedef Matrix<4, 4> Matrix4;

class Matrix34 : public Matrix<3, 4> {
 public:
  Matrix34();
  Matrix34(const Matrix4 &matrix4);
  explicit Matrix34(const myfloat fill);
  Matrix34(const myfloat *array);
  Matrix34(const Matrix34 &other);

  Matrix34 &operator *=(const Matrix34 &other);

 private:
  friend Vector3 operator *(const Matrix34 &matrix, const Vector3 &vector);
  friend Matrix34 operator *(const Matrix34 &a, const Matrix34 &b);
};

Vector3 operator *(const Matrix34 &matrix, const Vector3 &vector);
Matrix34 operator *(const Matrix34 &a, const Matrix34 &b);

class RotateTransform : public AffineTransform {
 public:
  RotateTransform(const myfloat angle, const Vector3UnitX &);
  RotateTransform(const myfloat angle, const Vector3UnitY &);
  RotateTransform(const myfloat angle, const Vector3UnitZ &);
  RotateTransform(const myfloat angle, const Vector3 &a);
};

class TranslateTransform : public AffineTransform {
 public:
  TranslateTransform(const myfloat x, const myfloat y, const myfloat z);
};

AffineTransform Scaling(const myfloat kx, const myfloat ky, const myfloat kz);

#endif

void TransformToEuler(const AffineTransform &transform, myfloat &roll, myfloat &pitch, myfloat &yaw);
void TransformToTranslate(const AffineTransform &transform, myfloat &x, myfloat &y, myfloat &z);

#endif // GRAPH_MATRIX4_H_
