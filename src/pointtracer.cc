#include <limits>
#include "common/math.h"
#include "triangletraverser.h"
#include "pointtracer.h"

using namespace std;

template <class Integral = myfloat> class PointTraceTraverser : public Traversable<Integral>, virtual public ZTraversable<Integral> {
 public:
  typedef typename ZTraversable<Integral>::DataType DataType;
  typedef typename ZTraversable<Integral>::HorizontalTraversable ZTraversableHorizontal;
  typedef typename Traversable<Integral>::template HorizontalTraversable<Integral> TraversableHorizontal;

  class HorizontalTraversable : public TraversableHorizontal, virtual public ZTraversableHorizontal {
   public:

    inline HorizontalTraversable(const ScreenLine<Integral> &la, const PointTraceTraverser &a,
                                 const ScreenLine<Integral> &lb, const PointTraceTraverser &b) :
      ZTraversableHorizontal(la, a, lb, b) {}

    virtual inline bool Process(myfloat *zp, const Integral, const Integral) {
      if (this->z() < *zp && this->z() >= 0) {
        *zp = this->z();
        return true;
      }
      return false;
    }

    inline void Advance() {
      ZTraversableHorizontal::Advance();
    }

    inline void Advance(const Integral value) {
      ZTraversableHorizontal::Advance(value);
    }
  };

  inline PointTraceTraverser(const ScreenLine<Integral> &line, const DataType &a, const DataType &b) :
    ZTraversable<Integral>(line, a, b) {}

  inline void Advance() {
    ZTraversable<Integral>::Advance();
  }

  inline void Advance(const Integral value) {
    ZTraversable<Integral>::Advance(value);
  }
};

PointTracer::PointTracer() : z_(std::numeric_limits<myfloat>::max()), x_(-1), y_(-1) {}

void PointTracer::Reset() {
  z_ = std::numeric_limits<myfloat>::max();
}

bool PointTracer::TraceNext(const IndexedTriangle &triangle, const Point3DVector &points) {
  return TriangleTraverser<PointTraceTraverser<>>::OnePoint(&z_, x_, y_,
                                                   points[triangle.points[0]], points[triangle.points[0]].z,
                                                   points[triangle.points[1]], points[triangle.points[1]].z,
                                                   points[triangle.points[2]], points[triangle.points[2]].z);
}

void PointTracer::set_point(unsigned int x, unsigned int y) {
  x_ = x;
  y_ = y;
  Reset();
}
