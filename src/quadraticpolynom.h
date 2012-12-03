#ifndef GRAPH_QUADRATICPOLYNOM_H_
#define GRAPH_QUADRATICPOLYNOM_H_

#include "myfloat.h"
#include "common/math.h"
#include "point3d.h"
#include "point2d.h"

struct QuadraticPolynom {
  QuadraticPolynom();
  QuadraticPolynom(const myfloat x1, const myfloat y1, const myfloat x2, const myfloat y2,
                   const myfloat x3, const myfloat y3);
  
  inline myfloat GetPoint(const myfloat x) {
    return a * Sqr(x) + b * x + c;
  }

  myfloat a;
  myfloat b;
  myfloat c;
};

struct Surface2D {
 public:
  Surface2D();

  template <class Point> Surface2D(const Point &a, const Point2D &na,
                                   const Point &b, const Point2D &nb,
                                   const Point &c, const Point2D &nc) :
    x_(a.x, na.x, b.x, nb.x, c.x, nc.x), y_(a.y, na.y, b.y, nb.y, c.y, nc.y) {}

  inline Point2D GetPoint(const myfloat x, const myfloat y) {
    return Point2D(x_.GetPoint(x), y_.GetPoint(y)); 
  }

 private:
  QuadraticPolynom x_;
  QuadraticPolynom y_;
};

struct Surface3D {
 public:
  Surface3D();

  template <class Point> Surface3D(const Point &a, const Point3D &na,
                                   const Point &b, const Point3D &nb,
                                   const Point &c, const Point3D &nc) :
    x_(a.x, na.x, b.x, nb.x, c.x, nc.x), y_(a.y, na.y, b.y, nb.y, c.y, nc.y),
    z_(a.z, na.z, b.z, nb.z, c.z, nc.z) {}

  inline Point3D GetPoint(const myfloat x, const myfloat y, const myfloat z) {
    return Point3D(x_.GetPoint(x), y_.GetPoint(y), z_.GetPoint(z)); 
  }

 private:
  QuadraticPolynom x_;
  QuadraticPolynom y_;
  QuadraticPolynom z_;
};

#endif // GRAPH_QUADRATICPOLYNOM_H_
