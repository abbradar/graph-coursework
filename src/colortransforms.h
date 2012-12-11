#ifndef GRAPH_COLORTRANSFORMS_H_
#define GRAPH_COLORTRANSFORMS_H_

#include "sdlobj/color.h"
#include "vector3.h"

inline sdlobj::Color VectorToColor(const Vector3 &vec) {
  int r = vec.x(), g = vec.y(), b = vec.z();
  if (r > 0xFF) r = 0xFF;
  if (g > 0xFF) g = 0xFF;
  if (b > 0xFF) b = 0xFF;
  return sdlobj::Color(r, g, b);
}

inline Vector3 ColorToVector(const sdlobj::Color &color) {
  return Vector3(color.r, color.g, color.b);
}

#endif // GRAPH_COLORTRANSFORMS_H_
