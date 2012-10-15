#ifndef GRAPH_POSITION_H_
#define GRAPH_POSITION_H_

#include "matrix4.h"
#include "point3d.h"
#include "myfloat.h"

/** Object position in 3D space.
 * Our space is positioned so starting user camera direction is parallel with x,
 * z goes up.
 * roll - [-Pi/2; Pi/2], pitch - [-Pi; Pi], yaw - [-Pi/2; Pi/2]
 */
struct Position {
  myfloat x = 0;
  myfloat y = 0;
  myfloat z = 0;
  myfloat roll = 0;
  myfloat pitch = 0;
  myfloat yaw = 0;

  Position();
  Position(myfloat x, myfloat y, myfloat z, myfloat roll = 0, myfloat pitch = 0, myfloat yaw = 0);

  Matrix4 GetMatrixFrom();
  Matrix4 GetMatrixTo();
  Matrix4 GetRotateMatrixFrom();
  Matrix4 GetRotateMatrixTo();
  Matrix4 GetTranslateMatrixFrom();
  Matrix4 GetTranslateMatrixTo();
};

#endif // GRAPH_POSITION_H_
