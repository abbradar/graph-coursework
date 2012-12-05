#ifndef GRAPH_POSITION_H_
#define GRAPH_POSITION_H_

#include "matrix4.h"
#include "vector3.h"
#include "myfloat.h"

/** Object position in 3D space.
 * Our space is positioned so starting user camera direction is parallel with x,
 * z goes up.
 * roll - [-Pi/2; Pi/2], pitch - [-Pi; Pi], yaw - [-Pi/2; Pi/2]
 */
struct Position {
  static const Vector3 kCameraDirection;

  myfloat x = 0;
  myfloat y = 0;
  myfloat z = 0;
  myfloat roll = 0;
  myfloat pitch = 0;
  myfloat yaw = 0;

  Position();
  Position(myfloat x, myfloat y, myfloat z, myfloat roll = 0, myfloat pitch = 0, myfloat yaw = 0);

  AffineTransform GetMatrixFrom() const;
  AffineTransform GetMatrixTo() const;
  AffineTransform GetRotateMatrixFrom() const;
  AffineTransform GetRotateMatrixTo() const;
  AffineTransform GetTranslateMatrixFrom() const;
  AffineTransform GetTranslateMatrixTo() const;
  Vector3 GetVector3() const;
};

#endif // GRAPH_POSITION_H_
