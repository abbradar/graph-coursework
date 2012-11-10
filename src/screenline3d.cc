#include <algorithm>
#include "screenline3d.h"

using namespace std;

ScreenLine3D::ScreenLine3D() = default;

ScreenLine3D::ScreenLine3D(const Point3D &a, const Point3D &b) : dx(0), dz(0) {
  y = (unsigned int)a.y;
  fy = (unsigned int)b.y;
  x = a.x;
  if (y != fy) {
    myfloat dy = fy - y; /*b->y - a->y*/
    dx = (b.x - a.x) / dy;
    dz = (b.z - a.z) / dy;
    z = a.z;
  } else {
    dx = 0;
    dz = 0;
    z = min(a.z, b.z);
  }
}
