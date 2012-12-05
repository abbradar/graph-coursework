#ifndef GRAPH_POINT3D_H_
#define GRAPH_POINT3D_H_

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

#include "matrix4.h"

class Vector3 {
 public:
  constexpr inline Vector3() : x_(0), y_(0), z_(0) {}
  constexpr inline Vector3(myfloat x, myfloat y, myfloat z) :
      x_(x), y_(y), z_(z) {}
  inline ~Vector3() = default;

  inline myfloat &x() {
    return x_;
  }

  inline myfloat &y() {
    return y_;
  }

  inline myfloat &z() {
    return z_;
  }

  inline const myfloat &x() const {
    return x_;
  }

  inline const myfloat &y() const {
    return y_;
  }

  inline const myfloat &z() const {
    return z_;
  }

  Vector3 &operator +=(const Vector3 &other);
  Vector3 &operator -=(const Vector3 &other);

  template <class T> Vector3 &operator *=(const T num) {
    x_ *= num;
    y_ *= num;
    z_ *= num;
    return *this;
  }

  template <class T> Vector3 &operator /=(const T num) {
    x_ /= num;
    y_ /= num;
    z_ /= num;
    return *this;
  }

  Vector3 cross(const Vector3 &b) const;
  myfloat dot(const Vector3 &b) const;

  static const Vector3UnitX &UnitX();
  static const Vector3UnitY &UnitY();
  static const Vector3UnitZ &UnitZ();

  myfloat norm() const;
  myfloat squaredNorm() const;

 private:
  myfloat x_;
  myfloat y_;
  myfloat z_;

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

#endif // GRAPH_POINT3D_H_
