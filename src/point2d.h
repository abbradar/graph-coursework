#ifndef GRAPH_POINT2D_H_
#define GRAPH_POINT2D_H_

#include "xparse/xdata.h"
#include "myfloat.h"

struct Point2D {
 public:
  myfloat x;
  myfloat y;

  Point2D();
  Point2D(myfloat x, myfloat y);

  Point2D &operator +=(const Point2D &other);
  Point2D &operator -=(const Point2D &other);

  static Point2D LoadFromXCoords2D(const xparse::XDataValue::NodeData &data);

 private:
  friend Point2D operator +(const Point2D &a, const Point2D &b);
  friend Point2D operator -(const Point2D &a, const Point2D &b);
};

Point2D operator +(const Point2D &a, const Point2D &b);
Point2D operator -(const Point2D &a, const Point2D &b);

#endif // GRAPH_POINT2D_H_
