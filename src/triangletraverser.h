#ifndef GRAPH_TRIANGLETRAVERSER_H_
#define GRAPH_TRIANGLETRAVERSER_H_

#include <type_traits>
#include <limits>
#include <algorithm>
#include "common/math.h"
#include "point2d.h"
#include "myfloat.h"

template <class Integral> class ScreenLine {
 public:
  inline ScreenLine() = default;
  
  inline ScreenLine(const Point2D &a, const Point2D &b) : y_(a.y), fy_(b.y),
          x_(a.x), dx_(0) {
    static_assert(std::is_arithmetic<Integral>::value, "Template type should be arithmetic.");
    Integral dy = fy_ - y_;
    if (dy != 0) {
      dx_ = (b.x - a.x) / dy;
    }
  }

  inline ~ScreenLine() = default;

  inline void Advance() {
    ++y_;
    x_ += dx_;
  }

  inline void Advance(const Integral value) {
    y_ += value;
    x_ += dx_ * value;
  }

  inline myfloat x() const {
    return x_;
  }

  inline Integral y() const {
    return y_;
  }

  inline Integral fy() const {
    return fy_;
  }

  inline myfloat dx() const {
    return dx_;
  }

 private:
  Integral y_;
  Integral fy_;
  myfloat x_;
  myfloat dx_;
};

template <class Integral> class Traversable {
 public:
  typedef Integral IntegralType;
  // There should also be:
  // typedef <smth> DataType
  //
  // inline Traversable(const ScreenLine<Integral> &line, const DataType &a,
  // const DataType &b);

  inline Traversable() {
    static_assert(std::is_arithmetic<Integral>::value, "Template type should be arithmetic.");
  }

  virtual inline ~Traversable() = default;

  template <class Context> class HorizontalTraversable {
   public:
    // There should also be:
    // inline HorizontalTraversable(const ScreenLine<Integral> &la, const Traversable<Integral> &a,
    //                              const ScreenLine<Integral> &lb, const Traversable<Integral> &b)
    inline HorizontalTraversable() {
      static_assert(std::is_arithmetic<Integral>::value, "Template type should be arithmetic.");
    }

    typedef Context ContextType;

    virtual inline void Advance() = 0;
    virtual inline void Advance(const Integral value) = 0;
    virtual inline bool Process(Context *context, const Integral x, const Integral y) = 0;
  };

  virtual inline void Advance() = 0;
  virtual inline void Advance(const Integral value) = 0;
};

template <class Integral> class ZTraversable {
 public:
  typedef Integral DataType;

  class HorizontalTraversable {
   public:
    inline HorizontalTraversable(const ScreenLine<Integral> &la, const ZTraversable<Integral> &a,
                                 const ScreenLine<Integral> &lb, const ZTraversable<Integral> &b) {
      static_assert(std::is_arithmetic<Integral>::value, "Template type should be arithmetic.");

      Integral dx = (Integral)lb.x() - (Integral)la.x();
      if (dx != 0) {
        z_ = std::min(a.z(), b.z());
        dz_ = 0;
      } else {
        z_ = a.z();
        dz_ = (b.z() - z_) / dx;
      }
    }

    inline void Advance() {
      z_ += dz_;
    }

    inline void Advance(const Integral value) {
      z_ += dz_ * value;
    }

    inline myfloat z() const {
      return z_;
    }

    inline myfloat dz() const {
      return dz_;
    }
   
   private:
    myfloat z_;
    myfloat dz_;
  };

  inline ZTraversable() = default;

  inline ZTraversable(const ScreenLine<Integral> &line, const DataType &a, const DataType &b) {
    static_assert(std::is_arithmetic<Integral>::value, "Template type should be arithmetic.");

    Integral dx = (Integral)line.fy() - (Integral)line.y();
    if (dx != 0) {
      z_ = a;
      dz_ = (b - a) / dx;
    } else {
      z_ = std::min(a, b);
    }
  }

  inline void Advance() {
    z_ += dz_;
  }

  inline void Advance(const Integral value) {
    z_ += dz_ * value;
  }

  inline myfloat z() const {
    return z_;
  }

  inline myfloat dz() const {
    return dz_;
  }

 private:
  myfloat z_;
  myfloat dz_;
};

template <class T> class TriangleTraverser {
 public:
  typedef typename T::HorizontalTraversable HorizontalTraversable;
  typedef typename HorizontalTraversable::ContextType ContextType;
  typedef typename T::DataType DataType;
  typedef typename T::IntegralType IntegralType;

  static constexpr size_t kLinesNum = 3;

  TriangleTraverser(const Point2D &a, const DataType &da,
                    const Point2D &b, const DataType &db,
                    const Point2D &c, const DataType &dc) :
      xmin(std::numeric_limits<myfloat>::max()),
      xmax(-std::numeric_limits<myfloat>::max()),
      horizontal(false), right_big(false) {    
    const Point2D *points_ptr[kLinesNum];
    points_ptr[0] = &a; points_ptr[1] = &b, points_ptr[2] = &c;
    const DataType *data_ptr[kLinesNum];
    data_ptr[0] = &da; data_ptr[1] = &db, data_ptr[2] = &dc;
    
    SortPoints(points_ptr, data_ptr);
    
    // find x edges
    for (size_t i = 0; i < kLinesNum; ++i) {
      WideBounds(points_ptr[i]->x, xmin, xmax);
    }

    if ((IntegralType)points_ptr[0]->y == (IntegralType)points_ptr[1]->y) {
      if (points_ptr[0]->x > points_ptr[1]->x) std::swap(points_ptr[0], points_ptr[1]);
      big = ScreenLine<IntegralType>(*points_ptr[0], *points_ptr[2]);
      tbig = T(big, *data_ptr[0], *data_ptr[2]);
      small1 = ScreenLine<IntegralType>(*points_ptr[1], *points_ptr[2]);
      tsmall1 = T(small1, *data_ptr[1], *data_ptr[2]);
      horizontal = true;
    } else {
      big = ScreenLine<IntegralType>(*points_ptr[0], *points_ptr[2]);
      tbig = T(big, *data_ptr[0], *data_ptr[2]);
      small1 = ScreenLine<IntegralType>(*points_ptr[0], *points_ptr[1]);
      tsmall1 = T(small1, *data_ptr[0], *data_ptr[1]);

      right_big = big.dx() > small1.dx();

      if (big.y() != big.fy()) {
        small2 = ScreenLine<IntegralType>(*points_ptr[1], *points_ptr[2]);
        tsmall2 = T(small2, *data_ptr[1], *data_ptr[2]);
      } else {
        horizontal = true;
      }
    }
  }

  void Traverse(ContextType *context) {
    ScreenLine<IntegralType> lbig = big, lsmall1 = small1;
    T ltbig = tbig, ltsmall1 = tsmall1;
    if (!right_big)
      TraverseLines(lbig, ltbig, lsmall1, ltsmall1, lsmall1.fy(), context);
    else
      TraverseLines(lsmall1, ltsmall1, lbig, ltbig, lsmall1.fy(), context);
    if (!horizontal) {
      ScreenLine<IntegralType> lsmall2 = small2;
      T ltsmall2 = tsmall2;
      if (!right_big)
        TraverseLines(lbig, ltbig, lsmall2, ltsmall2, lbig.fy(), context);
      else
        TraverseLines(lsmall2, ltsmall2, lbig, ltbig, lbig.fy(), context);
    }
  }

  bool Point(ContextType *context, const IntegralType x, const IntegralType y) {
    if (y < big.y() || y > big.fy() || x < xmin || x > xmax) return false;
    
    ScreenLine<IntegralType> lbig = big;
    T ltbig = tbig;
    if (!horizontal) {
      if (y <= small1.fy()) {
        ScreenLine<IntegralType> lsmall1 = small1;
        T ltsmall1 = tsmall1;
        if (!right_big)
          return PointLines(lbig, ltbig, lsmall1, ltsmall1, x, y, context);
        else
          return PointLines(lsmall1, ltsmall1, lbig, ltbig, x, y, context);
      } else {
        ScreenLine<IntegralType> lsmall2 = small2;
        T ltsmall2 = tsmall2;
        if (!right_big)
          return PointLines(lbig, ltbig, lsmall2, ltsmall2, x, y, context);
        else
          return PointLines(lsmall2, ltsmall2, lbig, ltbig, x, y, context);
      }
    } else {
      ScreenLine<IntegralType> lsmall1 = small1;
      T ltsmall1 = tsmall1;
      if (!right_big)
        return PointLines(lbig, ltbig, lsmall1, ltsmall1, x, y, context);
      else
        return PointLines(lsmall1, ltsmall1, lbig, ltbig, x, y, context);
    }
  }

  static void OneTraverse(ContextType *context, const Point2D &a, const DataType &da,
                          const Point2D &b, const DataType &db,
                          const Point2D &c, const DataType &dc) {
    const Point2D *points_ptr[kLinesNum];
    points_ptr[0] = &a; points_ptr[1] = &b, points_ptr[2] = &c;
    const DataType *data_ptr[kLinesNum];
    data_ptr[0] = &da; data_ptr[1] = &db, data_ptr[2] = &dc;
    
    SortPoints(points_ptr, data_ptr);
    
    if ((IntegralType)points_ptr[0]->y == (IntegralType)points_ptr[1]->y) {
      if (points_ptr[0]->x > points_ptr[1]->x) std::swap(points_ptr[0], points_ptr[1]);
      ScreenLine<IntegralType> a(*points_ptr[0], *points_ptr[2]);
      T ta(a, *data_ptr[0], *data_ptr[2]);
      ScreenLine<IntegralType> b(*points_ptr[1], *points_ptr[2]);
      T tb(b, *data_ptr[1], *data_ptr[2]);
      TraverseLines(a, ta, b, tb, a.fy(), context);
    } else {
      ScreenLine<IntegralType> big(*points_ptr[0], *points_ptr[2]);
      T tbig(big, *data_ptr[0], *data_ptr[2]);
      ScreenLine<IntegralType> small1(*points_ptr[0], *points_ptr[1]);
      T tsmall1(small1, *data_ptr[0], *data_ptr[1]);
      if (big.dx() <= small1.dx()) {
        TraverseLines(big, tbig, small1, tsmall1, small1.fy(), context);

        if (big.y() != big.fy()) {
          ScreenLine<IntegralType> small2(*points_ptr[1], *points_ptr[2]);
          T tsmall2(small2, *data_ptr[1], *data_ptr[2]);
          TraverseLines(big, tbig, small2, tsmall2, big.fy(), context);
        }
      } else {
        TraverseLines(small1, tsmall1, big, tbig, small1.fy(), context);

        if (big.y() != big.fy()) {
          ScreenLine<IntegralType> small2(*points_ptr[1], *points_ptr[2]);
          T tsmall2(small2, *data_ptr[1], *data_ptr[2]);
          TraverseLines(small2, tsmall2, big, tbig, big.fy(), context);
        }
      }
    }
  }

  static bool OnePoint(ContextType *context, const IntegralType x, const IntegralType y,
                       const Point2D &a, const DataType &da,
                       const Point2D &b, const DataType &db,
                       const Point2D &c, const DataType &dc) {
    const Point2D *points_ptr[kLinesNum];
    points_ptr[0] = &a; points_ptr[1] = &b, points_ptr[2] = &c;
    const DataType *data_ptr[kLinesNum];
    data_ptr[0] = &da; data_ptr[1] = &db, data_ptr[2] = &dc;
    
    SortPoints(points_ptr, data_ptr);

    // find x edges
    myfloat xmin = std::numeric_limits<myfloat>::max(),
            xmax = -std::numeric_limits<myfloat>::max();
    for (size_t i = 0; i < kLinesNum; ++i) {
      WideBounds(points_ptr[i]->x, xmin, xmax);
    }
    // clip by triangle bounds
    if (x < xmin || x > xmax || y < points_ptr[0]->y
        || y > points_ptr[2]->y) return false;

    if ((IntegralType)points_ptr[0]->y == (IntegralType)points_ptr[1]->y) {
      if (points_ptr[0]->x > points_ptr[1]->x) std::swap(points_ptr[0], points_ptr[1]);
      ScreenLine<IntegralType> a(*points_ptr[0], *points_ptr[2]);
      T ta = T(a, *data_ptr[0], *data_ptr[2]);
      ScreenLine<IntegralType> b(*points_ptr[1], *points_ptr[2]);
      T tb = T(b, *data_ptr[1], *data_ptr[2]);
      return PointLines(a, ta, b, tb, x, y, context);
    } else {
      ScreenLine<IntegralType> big(*points_ptr[0], *points_ptr[2]);
      T tbig(big, *data_ptr[0], *data_ptr[2]);
      ScreenLine<IntegralType> small1(*points_ptr[0], *points_ptr[1]);
      T tsmall1(small1, *data_ptr[0], *data_ptr[1]);
      if (big.dx() <= small1.dx()) {
        if (y <= small1.fy()) {
          return PointLines(big, tbig, small1, tsmall1, x, y, context);
        } else {
          ScreenLine<IntegralType> small2(*points_ptr[1], *points_ptr[2]);
          T tsmall2(small2, *data_ptr[1], *data_ptr[2]);
          return PointLines(big, tbig, small2, tsmall2, x, y, context);
        }
      } else {
        if (y <= small1.fy()) {
          return PointLines(small1, tsmall1, big, tbig, x, y, context);
        } else {
          ScreenLine<IntegralType> small2(*points_ptr[1], *points_ptr[2]);
          T tsmall2(small2, *data_ptr[1], *data_ptr[2]);
          return PointLines(small2, tsmall2, big, tbig, x, y, context);
        }
      }
    }
  }

 private:
  static inline void SortPoints(const Point2D **points_ptr, const DataType **data_ptr) {
    // sort them
    for (size_t i = kLinesNum - 1; i > 0; --i) {
      for (size_t j = 0; j < i; ++j) {
        if (points_ptr[j]->y > points_ptr[j + 1]->y) {
          std::swap(points_ptr[j], points_ptr[j + 1]);
          std::swap(data_ptr[j], data_ptr[j + 1]);
        }
      }
    }
  }

  static bool PointLines(ScreenLine<IntegralType> &a, T &ta,
                         ScreenLine<IntegralType> &b, T &tb,
                         const IntegralType x, const IntegralType y, ContextType *context) {
    a.Advance(y - a.y());
    ta.Advance(y - a.y());
    b.Advance(y - b.y());
    tb.Advance(y - b.y());
    if (x < a.x() || x > b.x()) return false;
    auto tr = HorizontalTraversable(a, ta, b, tb);
    tr.Advance(x - a.x());
    return tr.Process(context, x, y);
  }

  static void TraverseLines(ScreenLine<IntegralType> &a, T &ta,
                            ScreenLine<IntegralType> &b, T &tb,
                            const IntegralType fy, ContextType *context) {
    for (IntegralType y = a.y(); y < fy; ++y) {
      TraverseLine(a, ta, b, tb, context);

      // move line points
      a.Advance();
      //ta.Advance();
      b.Advance();
      //tb.Advance();
    }
    TraverseLine(a, ta, b, tb, context);
  }

  static inline void TraverseLine(ScreenLine<IntegralType> &a, T &ta,
                                  ScreenLine<IntegralType> &b, T &tb,
                                  ContextType *context) {
    if (a.x() <= b.x()) {
      auto tr = HorizontalTraversable(a, ta, b, tb);
      IntegralType y = a.y();
      for (IntegralType x = a.x(); x <= b.x(); ++x) {
        tr.Process(context, x, y);
        tr.Advance();
      }
    }
  }

  ScreenLine<IntegralType> big, small1, small2;
  T tbig, tsmall1, tsmall2;
  myfloat xmin, xmax;
  bool horizontal, right_big;
};

#endif // GRAPH_TRIANGLETRAVERSER_H_