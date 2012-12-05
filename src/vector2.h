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
  constexpr inline Vector2() : x_(0), y_(0) {}
  constexpr inline Vector2(myfloat x, myfloat y) : x_(x), y_(y) {}

  Vector2 &operator +=(const Vector2 &other);
  Vector2 &operator -=(const Vector2 &other);

  template <class T> Vector2 &operator *=(const T num) {
    x_ *= num;
    y_ *= num;
    return *this;
  }

  template <class T> Vector2 &operator /=(const T num) {
    x_ /= num;
    y_ /= num;
    return *this;
  }

  inline myfloat &x() {
    return x_;
  }

  inline myfloat &y() {
    return y_;
  }

  inline const myfloat &x() const {
    return x_;
  }

  inline const myfloat &y() const {
    return y_;
  }

  myfloat norm() const {
    return sqrt(squaredNorm());
  }

  myfloat squaredNorm() const {
    return Sqr(x_) + Sqr(y_);
  }

  Vector3 normalized() const {
    return *this / norm();
  }

 private:
  myfloat x_;
  myfloat y_;

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
