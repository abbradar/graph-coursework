#include <limits>
#include "common/math.h"
#include "screenline3d.h"
#include "pointtracer.h"

using namespace std;

PointTracer::PointTracer() : z_(std::numeric_limits<myfloat>::max()), x_(-1), y_(-1) {}

void PointTracer::Reset() {
  z_ = std::numeric_limits<myfloat>::max();
}

int PointTracer::TraceNext(const TriangleVector &triangles, const Point3DVector &points) {
  int result = -1;
  for (size_t triangle_i = 0; triangle_i < triangles.size(); ++triangle_i) {
    const IndexedTriangle &triangle = triangles[triangle_i];
    const Point3D *points_ptr[IndexedTriangle::kPointsSize];
    for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
      points_ptr[i] = &points[triangle.points[i]];
    }
    // sort them
    for (size_t i = IndexedTriangle::kPointsSize - 1; i > 0; --i) {
      for (size_t j = 0; j < i; ++j) {
        if (points_ptr[j]->y > points_ptr[j + 1]->y) swap(points_ptr[j], points_ptr[j + 1]);
      }
    }
    // find x edges
    myfloat xmin = std::numeric_limits<myfloat>::max(),
            xmax = -std::numeric_limits<myfloat>::max();
    for (size_t i = IndexedTriangle::kPointsSize - 1; i > 0; --i) {
      WideBounds(points_ptr[i]->x, xmin, xmax);
    }
    // clip by triangle bounds
    if (x_ < xmin || x_ > xmax || y_ < points_ptr[0]->y
        || y_ > points_ptr[2]->y) continue;

    if ((unsigned)points_ptr[0]->y == (unsigned)points_ptr[1]->y) {
      if (points_ptr[0]->x > points_ptr[1]->x) swap(points_ptr[0], points_ptr[1]);
      ScreenLine3D a = ScreenLine3D(*points_ptr[0], *points_ptr[2]);
      ScreenLine3D b = ScreenLine3D(*points_ptr[1], *points_ptr[2]);
      if (TraceLines(a, b)) return result;
    } else {
      ScreenLine3D big(*points_ptr[0], *points_ptr[2]);
      ScreenLine3D small1(*points_ptr[0], *points_ptr[1]);
      if (big.dx <= small1.dx) {
        if (y_ <= small1.fy) {
          if (TraceLines(big, small1)) result = triangle_i;
        } else {
          ScreenLine3D small2(*points_ptr[1], *points_ptr[2]);
          if (TraceLines(big, small2)) result = triangle_i;
        }
      } else {
        if (y_ <= small1.fy) {
          if (TraceLines(small1, big)) result = triangle_i;
        } else {
          ScreenLine3D small2(*points_ptr[1], *points_ptr[2]);
          if (TraceLines(small2, big)) result = triangle_i;
        }
      }
    }
  }
  return result;
}

void PointTracer::set_point(unsigned int x, unsigned int y) {
  x_ = x;
  y_ = y;
  Reset();
}

bool PointTracer::TraceLines(ScreenLine3D &a, ScreenLine3D &b) {
  a.Advance(y_ - a.y);
  b.Advance(y_ - b.y);
  if (x_ < a.x || x_ > b.x) return false;
  myfloat z = (b.z - a.z) / (b.x - a.x) * (x_ - a.x);
  if (z < z_) {
    z_ = z;
    return true;
  }
  return false;
}
