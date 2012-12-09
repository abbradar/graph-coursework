#ifndef GRAPH_VECTOR2_H_
#define GRAPH_VECTOR2_H_

#include "myfloat.h"
#include "config.h"

#ifdef USE_EIGEN

#include "Eigen/Core"

typedef Eigen::Matrix<myfloat, 2, 1> Vector2;

#else

#include "common/math.h"

class Vector2 {
 public:
  constexpr inline Vector2(const myfloat x, const myfloat y) {
    this->x() = x;
    this->y() = y;
  }
  constexpr inline Vector2() : Vector2(0, 0) {}

  Vector2 &operator +=(const Vector2 &other);
  Vector2 &operator -=(const Vector2 &other);

  template <class T> Vector2 &operator *=(const T num) {
    x() *= num;
    y() *= num;
    return *this;
  }

  template <class T> Vector2 &operator /=(const T num) {
    x() /= num;
    y() /= num;
    return *this;
  }

  inline Vector2 operator -() const {
    return Vector2(-x(), -y());
  }

  inline myfloat &x() {
    return data_.dim.x;
  }

  inline myfloat &y() {
    return data_.dim.y;
  }

  inline myfloat x() const {
    return data_.dim.x;
  }

  inline myfloat y() const {
    return data_.dim.y;
  }

  inline myfloat operator ()(const size_t i) const {
    return data_.arr[i];
  }

  inline myfloat &operator ()(const size_t i) {
    return data_.arr[i];
  }

  myfloat norm() const {
    return sqrt(squaredNorm());
  }

  myfloat squaredNorm() const {
    return Sqr(x_) + Sqr(y_);
  }

  Vector2 normalized() const {
    return Vector2(*this) /= norm();
  }

  inline constexpr size_t cols() {
    return 1;
  }

  inline constexpr size_t rows() {
    return kDimension;
  }

  inline const

 private:
  static constexpr size_t kDimension = 2;

  union {
    struct {
      myfloat x;
      myfloat y;
    } dim;
    myfloat arr[kDimension];
  } data_;

  friend Vector2 operator +(const Vector2 &a, const Vector2 &b);
  friend Vector2 operator -(const Vector2 &a, const Vector2 &b);
};

Vector2 operator +(const Vector2 &a, const Vector2 &b);
Vector2 operator -(const Vector2 &a, const Vector2 &b);

template <class T> Vector2 operator *(const Vector2 &point, const T num) {
  Vector2 p(point);
  p *= num;
  return p;
}

template <class T> Vector2 operator *(const T num, const Vector2 &point) {
  Vector2 p(point);
  p *= num;
  return p;
}

template <class T> Vector2 operator /(const Vector2 &point, const T num) {
  Vector2 p(point);
  p /= num;
  return p;
}

#endif

myfloat Angle(const Vector2 &vec);
myfloat NormAngle(const Vector2 &vec);
myfloat Angle(const Vector2 &a, const Vector2 &b);
myfloat NormAngle(const Vector2 &a, const Vector2 &b);

#endif // GRAPH_VECTOR2_H_
