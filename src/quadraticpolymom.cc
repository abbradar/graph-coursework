#include "quadraticpolynom.h"

QuadraticPolynom::QuadraticPolynom() : a(0), b(0), c(0) {}

QuadraticPolynom::QuadraticPolynom(const myfloat x1, const myfloat y1, const myfloat x2, const myfloat y2,
                                   const myfloat x3, const myfloat y3) {
  QuadraticInterpolation(x1, y1, x2, y2, x3, y3, a, b, c);
}

Surface2D::Surface2D() = default;

Surface3D::Surface3D() = default;

