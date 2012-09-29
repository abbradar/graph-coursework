#ifndef GRAPH_MATRIX4_H_
#define GRAPH_MATRIX4_H_

class Matrix4;

#include "point3d.h"

class Matrix4 {
 public:
  static const int kMatrixWidth;
  static const int kMatrixHeight;

  Matrix4();
  Matrix4(float fill);
  Matrix4(const Matrix4 &other);
  ~Matrix4();

  Matrix4 &operator =(const Matrix4 &other);

  inline float &at(int x, int y) {
    return matrix_[y * kMatrixWidth + x];
  }

  static Matrix4 Translate(float x, float y, float z);
  static Matrix4 RotateX(float a);
  static Matrix4 RotateY(float a);
  static Matrix4 RotateZ(float a);
  static Matrix4 Scale(float x, float y, float z);
  static Matrix4 Perspective(float n, float f);

  Matrix4 &operator +=(const Matrix4 &other);
  Matrix4 &operator -=(const Matrix4 &other);
  Matrix4 &operator *=(const Matrix4 &other);

 private:
  float* matrix_;

  friend Matrix4 operator +(const Matrix4 &a, const Matrix4 &b);
  friend Matrix4 operator -(const Matrix4 &a, const Matrix4 &b);
  friend Matrix4 operator *(const Matrix4 &a, const Matrix4 &b);
  friend Point3D operator *(const Matrix4 &matrix, const Point3D &point);
};

Matrix4 operator +(const Matrix4 &a, const Matrix4 &b);
Matrix4 operator -(const Matrix4 &a, const Matrix4 &b);
Matrix4 operator *(const Matrix4 &a, const Matrix4 &b);

#endif // GRAPH_MATRIX4_H_
