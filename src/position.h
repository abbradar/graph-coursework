#ifndef GRAPH_POSITION_H_
#define GRAPH_POSITION_H_

#include "matrix4.h"
#include "my_float.h"

/** Object position in 3D space.
 * Our space is positioned so starting user camera direction is parallel with x,
 * z goes up.
 * roll - [-Pi/2; Pi/2], pitch - [-Pi; Pi], yaw - [-Pi/2; Pi/2]
 */
struct Position {
  my_float x = 0;
  my_float y = 0;
  my_float z = 0;
  my_float pitch = 0;
  my_float yaw = 0;
  my_float roll = 0;

  Position();
  Position(my_float x, my_float y, my_float z, my_float pitch = 0, my_float yaw = 0, my_float roll = 0);

  Matrix4 GetMatrixFrom();
  Matrix4 GetMatrixTo();
};

#endif // GRAPH_POSITION_H_
