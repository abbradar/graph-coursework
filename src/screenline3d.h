#ifndef GRAPH_SCREENLINE3D_H_
#define GRAPH_SCREENLINE3D_H_

#include "point3d.h"
#include "myfloat.h"

struct ScreenLine3D {
 public:
  ScreenLine3D();
  ScreenLine3D(const Point3D &a, const Point3D &b);

  inline void Advance() {
    ++y;
    x += dx;
    z += dz;
  }

  inline void Advance(int value) {
    y += value;
    x += dx * value;
    z += dz * value;
  }

  myfloat x;
  unsigned int y;
  myfloat z;
  unsigned int fy;
  myfloat dx;
  myfloat dz;
};

#endif // GRAPH_SCREENLINE3D_H_
