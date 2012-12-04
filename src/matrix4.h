#ifndef GRAPH_MATRIX4_H_
#define GRAPH_MATRIX4_H_

class Matrix4;

#include <cstring>
#include "xparse/xdata.h"
#include "common/debug.h"
#include "common/exception.h"
#include "point3d.h"
#include "myfloat.h"

class Matrix4 {
 public:
  static const size_t kMatrixWidth;
  static const size_t kMatrixHeight;

  Matrix4();
  Matrix4(myfloat fill);
  Matrix4(const myfloat *array);
  Matrix4(const Matrix4 &other);
  ~Matrix4();

  Matrix4 &operator =(const Matrix4 &other);

  inline myfloat &at(size_t x, size_t y) {
#if DEBUG_LEVEL >= 4
    if (x > kMatrixWidth || y > kMatrixHeight) {
      throw Exception("Matrix index is out of bounds.");
    }
#endif
    return matrix_[y * kMatrixWidth + x];
  }

  inline myfloat *data() {
    return matrix_;
  }

  Matrix4 Transpose();

  static Matrix4 Translate(myfloat x, myfloat y, myfloat z);
  static Matrix4 RotateX(myfloat a);
  static Matrix4 RotateY(myfloat a);
  static Matrix4 RotateZ(myfloat a);
  static Matrix4 Scale(myfloat x, myfloat y, myfloat z);
  static Matrix4 Identity();

  static Matrix4 LoadFromXTransformMatrix(xparse::XData *data);

  Matrix4 &operator +=(const Matrix4 &other);
  Matrix4 &operator -=(const Matrix4 &other);
  Matrix4 &operator *=(const Matrix4 &other);

  void ToRotate(myfloat &roll, myfloat &pitch, myfloat &yaw);
  void ToTranslate(myfloat &x, myfloat &y, myfloat &z);

 private:
  myfloat *matrix_;

  friend Matrix4 operator +(const Matrix4 &a, const Matrix4 &b);
  friend Matrix4 operator -(const Matrix4 &a, const Matrix4 &b);
  friend Matrix4 operator *(const Matrix4 &a, const Matrix4 &b);
  friend Point3D operator *(const Matrix4 &matrix, const Point3D &point);
};

Matrix4 operator +(const Matrix4 &a, const Matrix4 &b);
Matrix4 operator -(const Matrix4 &a, const Matrix4 &b);
Matrix4 operator *(const Matrix4 &a, const Matrix4 &b);

#endif // GRAPH_MATRIX4_H_
