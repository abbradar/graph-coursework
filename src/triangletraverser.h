#ifndef GRAPH_TRIANGLETRAVERSER_H_
#define GRAPH_TRIANGLETRAVERSER_H_

#include <type_traits>
#include <limits>
#include <algorithm>
#include "common/math.h"
#include "myfloat.h"

template <class Integral> class ScreenLine {
 public:
  inline ScreenLine() = default;
  
  template <class Vector> inline ScreenLine(const Vector &a, const Vector &b) : y_(a.y()), fy_(b.y()),
      x_(a.x()), dx_(0) {
    static_assert(std::is_arithmetic<Integral>::value, "Template type should be arithmetic.");
    Integral dy = fy_ - y_;
    if (dy != 0) {
      dx_ = (b.x() - a.x()) / dy;
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

  inline Traversable() {
    static_assert(std::is_arithmetic<Integral>::value, "Template type should be arithmetic.");
  }

  template <class Context> class HorizontalTraversable {
   public:
    inline HorizontalTraversable() {
      static_assert(std::is_arithmetic<Integral>::value, "Template type should be arithmetic.");
    }

    typedef Context ContextType;
  };
};

template <class Integral> class ZTraversable {
 public:
  typedef myfloat DataType;

  class HorizontalTraversable {
   public:
    inline HorizontalTraversable(const ScreenLine<Integral> &la, const ZTraversable<Integral> &a,
                                 const ScreenLine<Integral> &lb, const ZTraversable<Integral> &b,
                                 const myfloat dx) {
      static_assert(std::is_arithmetic<Integral>::value, "Template type should be arithmetic.");

      if (dx != 0) {
        z_ = a.z();
        dz_ = (b.z() - z_) / dx;
      } else {
        z_ = std::min(a.z(), b.z());
        dz_ = 0;
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

  inline ZTraversable(const ScreenLine<Integral> &line, const myfloat dy, const DataType &a, const DataType &b) {
    static_assert(std::is_arithmetic<Integral>::value, "Template type should be arithmetic.");
    if (dy != 0) {
      z_ = a;
      dz_ = (b - a) / dy;
    } else {
      z_ = std::min(a, b);
      dz_ = 0;
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

  template <class Vector> TriangleTraverser(const Vector &a, const DataType &da,
                    const Vector &b, const DataType &db,
                    const Vector &c, const DataType &dc) :
      xmin_(std::numeric_limits<myfloat>::max()),
      xmax_(-std::numeric_limits<myfloat>::max()),
      horizontal_(false), right_big_(false) {
    const Vector *points_ptr[kLinesNum];
    points_ptr[0] = &a; points_ptr[1] = &b, points_ptr[2] = &c;
    const DataType *data_ptr[kLinesNum];
    data_ptr[0] = &da; data_ptr[1] = &db, data_ptr[2] = &dc;
    
    SortPoints(points_ptr, data_ptr);
    
    // find x edges
    for (size_t i = 0; i < kLinesNum; ++i) {
      WideBounds(points_ptr[i]->x(), xmin_, xmax_);
    }

    if ((IntegralType)points_ptr[0]->y() == (IntegralType)points_ptr[1]->y()) {
      if (points_ptr[0]->x() > points_ptr[1]->x()) {
        std::swap(points_ptr[0], points_ptr[1]);
        std::swap(data_ptr[0], data_ptr[1]);
      }
      big_ = ScreenLine<IntegralType>(*points_ptr[0], *points_ptr[2]);
      tbig_ = T(big_, big_.fy() - big_.y(), *data_ptr[0], *data_ptr[2]);
      small1_ = ScreenLine<IntegralType>(*points_ptr[1], *points_ptr[2]);
      tsmall1_ = T(small1_, small1_.fy() - small1_.y(), *data_ptr[1], *data_ptr[2]);
      horizontal_ = true;
    } else {
      big_ = ScreenLine<IntegralType>(*points_ptr[0], *points_ptr[2]);
      tbig_ = T(big_, big_.fy() - big_.y(), *data_ptr[0], *data_ptr[2]);
      small1_ = ScreenLine<IntegralType>(*points_ptr[0], *points_ptr[1]);
      tsmall1_ = T(small1_, small1_.fy() - small1_.y(), *data_ptr[0], *data_ptr[1]);

      right_big_ = big_.dx() > small1_.dx();

      if (big_.y() != big_.fy()) {
        small2_ = ScreenLine<IntegralType>(*points_ptr[1], *points_ptr[2]);
        tsmall2_ = T(small2_, small2_.fy() - small2_.y(), *data_ptr[1], *data_ptr[2]);
      } else {
        horizontal_ = true;
      }
    }
  }

  void Traverse(ContextType *context) {
    ScreenLine<IntegralType> lbig = big_, lsmall1 = small1_;
    T ltbig = tbig_, ltsmall1 = tsmall1_;
    if (!right_big_)
      TraverseLines(lbig, ltbig, lsmall1, ltsmall1, lsmall1.fy(), context);
    else
      TraverseLines(lsmall1, ltsmall1, lbig, ltbig, lsmall1.fy(), context);
    if (!horizontal_) {
      ScreenLine<IntegralType> lsmall2 = small2_;
      T ltsmall2 = tsmall2_;
      if (!right_big_)
        TraverseLines(lbig, ltbig, lsmall2, ltsmall2, lbig.fy(), context);
      else
        TraverseLines(lsmall2, ltsmall2, lbig, ltbig, lbig.fy(), context);
    }
  }

  template <bool kContinue> bool Point(ContextType *context, const IntegralType x, const IntegralType y) {
    if (!kContinue && (y < big_.y() || y > big_.fy() || x < xmin_ || x > xmax_)) return false;
    
    ScreenLine<IntegralType> lbig = big_;
    T ltbig = tbig_;
    if (!horizontal_) {
      if (y <= small1_.fy()) {
        ScreenLine<IntegralType> lsmall1 = small1_;
        T ltsmall1 = tsmall1_;
        if (!right_big_)
          return PointLines<kContinue>(lbig, ltbig, lsmall1, ltsmall1, x, y, context);
        else
          return PointLines<kContinue>(lsmall1, ltsmall1, lbig, ltbig, x, y, context);
      } else {
        ScreenLine<IntegralType> lsmall2 = small2_;
        T ltsmall2 = tsmall2_;
        if (!right_big_)
          return PointLines<kContinue>(lbig, ltbig, lsmall2, ltsmall2, x, y, context);
        else
          return PointLines<kContinue>(lsmall2, ltsmall2, lbig, ltbig, x, y, context);
      }
    } else {
      ScreenLine<IntegralType> lsmall1 = small1_;
      T ltsmall1 = tsmall1_;
      if (!right_big_)
        return PointLines<kContinue>(lbig, ltbig, lsmall1, ltsmall1, x, y, context);
      else
        return PointLines<kContinue>(lsmall1, ltsmall1, lbig, ltbig, x, y, context);
    }
  }

  template <class Vector> static void OneTraverse(ContextType *context, const Vector &a, const DataType &da,
                          const Vector &b, const DataType &db,
                          const Vector &c, const DataType &dc) {
    const Vector *points_ptr[kLinesNum];
    points_ptr[0] = &a; points_ptr[1] = &b, points_ptr[2] = &c;
    const DataType *data_ptr[kLinesNum];
    data_ptr[0] = &da; data_ptr[1] = &db, data_ptr[2] = &dc;
    
    SortPoints(points_ptr, data_ptr);
    
    if ((IntegralType)points_ptr[0]->y() == (IntegralType)points_ptr[1]->y()) {
      if (points_ptr[0]->x() > points_ptr[1]->x()) {
        std::swap(points_ptr[0], points_ptr[1]);
        std::swap(data_ptr[0], data_ptr[1]);
      }
      ScreenLine<IntegralType> a(*points_ptr[0], *points_ptr[2]);
      T ta(a, a.fy() - a.y(), *data_ptr[0], *data_ptr[2]);
      ScreenLine<IntegralType> b(*points_ptr[1], *points_ptr[2]);
      T tb(b, b.fy() - b.y(), *data_ptr[1], *data_ptr[2]);
      TraverseLines(a, ta, b, tb, a.fy(), context);
    } else {
      ScreenLine<IntegralType> big(*points_ptr[0], *points_ptr[2]);
      T tbig(big, big.fy() - big.y(), *data_ptr[0], *data_ptr[2]);
      ScreenLine<IntegralType> small1(*points_ptr[0], *points_ptr[1]);
      T tsmall1(small1, small1.fy() - small1.y(), *data_ptr[0], *data_ptr[1]);
      if (big.dx() <= small1.dx()) {
        TraverseLines(big, tbig, small1, tsmall1, small1.fy(), context);

        if (big.y() != big.fy()) {
          ScreenLine<IntegralType> small2(*points_ptr[1], *points_ptr[2]);
          T tsmall2(small2, small2.fy() - small2.y(), *data_ptr[1], *data_ptr[2]);
          TraverseLines(big, tbig, small2, tsmall2, big.fy(), context);
        }
      } else {
        TraverseLines(small1, tsmall1, big, tbig, small1.fy(), context);

        if (big.y() != big.fy()) {
          ScreenLine<IntegralType> small2(*points_ptr[1], *points_ptr[2]);
          T tsmall2(small2, small2.fy() - small2.y(), *data_ptr[1], *data_ptr[2]);
          TraverseLines(small2, tsmall2, big, tbig, big.fy(), context);
        }
      }
    }
  }

  template <class Vector, bool kContinue>
    static bool OnePoint(ContextType *context, const IntegralType x, const IntegralType y,
                       const Vector &a, const DataType &da,
                       const Vector &b, const DataType &db,
                       const Vector &c, const DataType &dc) {
    const Vector *points_ptr[kLinesNum];
    points_ptr[0] = &a; points_ptr[1] = &b, points_ptr[2] = &c;
    const DataType *data_ptr[kLinesNum];
    data_ptr[0] = &da; data_ptr[1] = &db, data_ptr[2] = &dc;

    SortPoints(points_ptr, data_ptr);

    // find x edges
    if (!kContinue) {
      myfloat xmin = std::numeric_limits<myfloat>::max(),
              xmax = -std::numeric_limits<myfloat>::max();
      for (size_t i = 0; i < kLinesNum; ++i) {
        WideBounds(points_ptr[i]->x(), xmin, xmax);
      }
      // clip by triangle bounds
      if (x < xmin || x > xmax || y < points_ptr[0]->y()
          || y > points_ptr[2]->y()) return false;
    }

    if ((IntegralType)points_ptr[0]->y() == (IntegralType)points_ptr[1]->y()) {
      if (points_ptr[0]->x() > points_ptr[1]->x()) {
        std::swap(points_ptr[0], points_ptr[1]);
        std::swap(data_ptr[0], data_ptr[1]);
      }
      ScreenLine<IntegralType> a(*points_ptr[0], *points_ptr[2]);
      T ta = T(a, a.fy() - a.y(), *data_ptr[0], *data_ptr[2]);
      ScreenLine<IntegralType> b(*points_ptr[1], *points_ptr[2]);
      T tb = T(b, b.fy() - b.y(), *data_ptr[1], *data_ptr[2]);
      return PointLines<kContinue>(a, ta, b, tb, x, y, context);
    } else {
      ScreenLine<IntegralType> big(*points_ptr[0], *points_ptr[2]);
      T tbig(big, big.fy() - big.y(), *data_ptr[0], *data_ptr[2]);
      ScreenLine<IntegralType> small1(*points_ptr[0], *points_ptr[1]);
      T tsmall1(small1, small1.fy() - small1.y(), *data_ptr[0], *data_ptr[1]);
      if (big.dx() <= small1.dx()) {
        if (y <= small1.fy()) {
          return PointLines<kContinue>(big, tbig, small1, tsmall1, x, y, context);
        } else {
          ScreenLine<IntegralType> small2(*points_ptr[1], *points_ptr[2]);
          T tsmall2(small2, small2.fy() - small2.y(), *data_ptr[1], *data_ptr[2]);
          return PointLines<kContinue>(big, tbig, small2, tsmall2, x, y, context);
        }
      } else {
        if (y <= small1.fy()) {
          return PointLines<kContinue>(small1, tsmall1, big, tbig, x, y, context);
        } else {
          ScreenLine<IntegralType> small2(*points_ptr[1], *points_ptr[2]);
          T tsmall2(small2, small2.fy() - small2.y(), *data_ptr[1], *data_ptr[2]);
          return PointLines<kContinue>(small2, tsmall2, big, tbig, x, y, context);
        }
      }
    }

    return false;
  }

 private:
  template <class Vector> static inline void SortPoints(const Vector **points_ptr, const DataType **data_ptr) {
    // sort them
    for (size_t i = kLinesNum - 1; i > 0; --i) {
      for (size_t j = 0; j < i; ++j) {
        if (points_ptr[j]->y() > points_ptr[j + 1]->y()) {
          std::swap(points_ptr[j], points_ptr[j + 1]);
          std::swap(data_ptr[j], data_ptr[j + 1]);
        }
      }
    }
  }

  template <bool kContinue> static bool PointLines(ScreenLine<IntegralType> &a, T &ta,
                         ScreenLine<IntegralType> &b, T &tb,
                         const IntegralType x, const IntegralType y, ContextType *context) {
    myfloat day = y - a.y();
    a.Advance(day);
    ta.Advance(day);
    myfloat dby = y - b.y();
    b.Advance(dby);
    tb.Advance(dby);
    if (!kContinue && (x < a.x() || x > b.x())) return false;
    auto tr = HorizontalTraversable(context, a, ta, b, tb);
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
      ta.Advance();
      b.Advance();
      tb.Advance();
    }
    TraverseLine(a, ta, b, tb, context);
  }

  static inline void TraverseLine(ScreenLine<IntegralType> &a, T &ta,
                                  ScreenLine<IntegralType> &b, T &tb,
                                  ContextType *context) {
    if (a.x() <= b.x()) {
      IntegralType y = a.y();
      IntegralType bx = b.x();
      auto tr = HorizontalTraversable(context, a, ta, b, tb, bx - y);
      for (IntegralType x = a.x(); x < bx; ++x) {
        tr.Process(context, x, y);
        tr.Advance();
      }
      tr.Process(context, bx, y);
    }
  }

  ScreenLine<IntegralType> big_, small1_, small2_;
  T tbig_, tsmall1_, tsmall2_;
  myfloat xmin_, xmax_;
  bool horizontal_, right_big_;
};

#endif // GRAPH_TRIANGLETRAVERSER_H_
