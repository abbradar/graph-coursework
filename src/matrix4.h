#ifndef GRAPH_MATRIX4_H_
#define GRAPH_MATRIX4_H_

class Matrix4;

#include "point3d.h"
#include "my_float.h"

class Matrix4 {
 public:
  static const int kMatrixWidth;
  static const int kMatrixHeight;

  Matrix4();
  Matrix4(my_float fill);
  Matrix4(const my_float *array);
  Matrix4(const Matrix4 &other);
  ~Matrix4();

  Matrix4 &operator =(const Matrix4 &other);

  inline my_float &at(int x, int y) {
    return matrix_[y * kMatrixWidth + x];
  }

  static Matrix4 Translate(my_float x, my_float y, my_float z);
  static Matrix4 RotateX(my_float a);
  static Matrix4 RotateY(my_float a);
  static Matrix4 RotateZ(my_float a);
  static Matrix4 Scale(my_float x, my_float y, my_float z);
  static Matrix4 Perspective(my_float ex, my_float ey, my_float ez);

  Matrix4 &operator +=(const Matrix4 &other);
  Matrix4 &operator -=(const Matrix4 &other);
  Matrix4 &operator *=(const Matrix4 &other);

 private:
  my_float* matrix_;

  friend Matrix4 operator +(const Matrix4 &a, const Matrix4 &b);
  friend Matrix4 operator -(const Matrix4 &a, const Matrix4 &b);
  friend Matrix4 operator *(const Matrix4 &a, const Matrix4 &b);
  friend Point3D operator *(const Matrix4 &matrix, const Point3D &point);
};

Matrix4 operator +(const Matrix4 &a, const Matrix4 &b);
Matrix4 operator -(const Matrix4 &a, const Matrix4 &b);
Matrix4 operator *(const Matrix4 &a, const Matrix4 &b);

#endif // GRAPH_MATRIX4_H_
