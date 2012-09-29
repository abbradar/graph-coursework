#ifndef GRAPH_POSITION_H_
#define GRAPH_POSITION_H_

#include "matrix4.h"

/** Object position in 3D space.
 * Pitch - [-Pi/2; Pi/2], yaw - [-Pi; Pi], roll - [-Pi/2; Pi/2]
 */
struct Position {
  float x = 0;
  float y = 0;
  float z = 0;
  float pitch = 0;
  float yaw = 0;
  float roll = 0;

  Matrix4 GetTransformMatrix();
};

#endif // GRAPH_POSITION_H_
