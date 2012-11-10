#ifndef GRAPH_POINTTRACER_H_
#define GRAPH_POINTTRACER_H_

#include "model.h"
#include "myfloat.h"

class PointTracer {
 public:
  PointTracer();

  void Reset();
  IndexedTriangle TraceNext(const TriangleVector &triangles, const PointVector &points);

  inline unsigned int x() {
    return x_;
  }

  inline unsigned int y() {
    return y_;
  }

  inline myfloat z() {
    return z_;
  }

  void set_point(unsigned int x, unsigned int y);

 private:
  myfloat z_;
  unsigned int x_, y_;
};

#endif // GRAPH_POINTTRACER_H_
