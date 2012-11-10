#include <limits>
#include "pointtracer.h"

using namespace std;

PointTracer::PointTracer() : z_(std::numeric_limits<myfloat>::max()), x_(-1), y_(-1) {}

void PointTracer::Reset() {
  z_ = std::numeric_limits<myfloat>::max();
}

IndexedTriangle PointTracer::TraceNext(const TriangleVector &triangles, const PointVector &points) {
  for (auto &triangle : triangles) {

  }
}

void PointTracer::set_point(unsigned int x, unsigned int y) {
  x_ = x;
  y_ = y;
  Reset();
}
