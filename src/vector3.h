#ifndef GRAPH_VECTOR3_H_
#define GRAPH_VECTOR3_H_

#include "myfloat.h"
#include "config.h"

#ifdef USE_EIGEN

#include <Eigen/Core>

typedef Eigen::Matrix<myfloat, 3, 1> Vector3;

#else

class Vector3;
class Vector3UnitX;
class Vector3UnitY;
class Vector3UnitZ;

#include "common/math.h"
#include "matrix4.h"

class Vector3 {
 public:
  constexpr inline Vector3(const myfloat x, const myfloat y, const myfloat z) {
    this->x() = x;
    this->y() = y;
    this->z() = z;
  }

  constexpr inline Vector3() : Vector3(0, 0, 0) {}

  inline ~Vector3() = default;

  inline myfloat &x() {
    return data_.dim.x;
  }

  inline myfloat &y() {
    return data_.dim.y;
  }

  inline myfloat &z() {
    return data_.dim.z;
  }

  inline myfloat x() const {
    return data_.dim.x;
  }

  inline myfloat y() const {
    return data_.dim.y;
  }

  inline myfloat z() const {
    return data_.dim.z;
  }

  inline myfloat operator ()(const size_t i) const {
    return data_.arr[i];
  }

  inline myfloat &operator ()(const size_t i) {
    return data_.arr[i];
  }

  Vector3 &operator +=(const Vector3 &other);
  Vector3 &operator -=(const Vector3 &other);

  template <class T> Vector3 &operator *=(const T num) {
    x() *= num;
    y() *= num;
    z() *= num;
    return *this;
  }

  template <class T> Vector3 &operator /=(const T num) {
    x() /= num;
    y() /= num;
    z() /= num;
    return *this;
  }

  inline Vector3 operator -() const {
    return Vector3(-x(), -y(), -z());
  }

  Vector3 cross(const Vector3 &b) const;
  myfloat dot(const Vector3 &b) const;

  static const Vector3UnitX &UnitX();
  static const Vector3UnitY &UnitY();
  static const Vector3UnitZ &UnitZ();

  myfloat norm() const {
    return sqrt(squaredNorm());
  }

  myfloat squaredNorm() const {
    return Sqr(x()) + Sqr(y()) + Sqr(z());
  }

  Vector3 normalized() const {
    return Vector3(*this) /= norm();
  }

  inline constexpr size_t cols() {
    return 1;
  }

  inline constexpr size_t rows() {
    return kDimension;
  }

 private:
  static constexpr size_t kDimension = 3;

  union {
    struct {
      myfloat x_;
      myfloat y_;
      myfloat z_;
    } dim;
    myfloat arr[kDimension];
  } data_;

  friend Vector3 operator +(const Vector3 &a, const Vector3 &b);
  friend Vector3 operator -(const Vector3 &a, const Vector3 &b);
  friend Vector3 operator *(const AffineTransform &matrix, const Vector3 &point);
};

Vector3 operator +(const Vector3 &a, const Vector3 &b);
Vector3 operator -(const Vector3 &a, const Vector3 &b);

template <class T> Vector3 operator *(const Vector3 &point, const T num) {
  Vector3 p(point);
  p *= num;
  return p;
}

template <class T> Vector3 operator *(const T num, const Vector3 &point) {
  Vector3 p(point);
  p *= num;
  return p;
}

template <class T> Vector3 operator /(const Vector3 &point, const T num) {
  Vector3 p(point);
  p /= num;
  return p;
}

class Vector3UnitX : public Vector3 {
 public:
  inline Vector3UnitX() : Vector3(1, 0, 0) {}
};

class Vector3UnitY : public Vector3 {
 public:
  inline Vector3UnitY() : Vector3(0, 1, 0) {}
};

class Vector3UnitZ : public Vector3 {
 public:
  inline Vector3UnitZ() : Vector3(0, 0, 1) {}
};

#endif

void AxisToEuler(const Vector3 &p, const myfloat angle, myfloat &roll, myfloat &pitch, myfloat &yaw);

inline Vector3 Reflection(const Vector3 &a, const Vector3 &n) {
  return 2 * a.dot(n) * n - a;
}

#endif // GRAPH_VECTOR3_H_
