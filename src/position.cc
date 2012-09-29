#include "position.h"

Matrix4 Position::GetTransformMatrix() {
  Matrix4 transform = Matrix4::Translate(x, y, z);
  transform *= Matrix4::RotateX(pitch);
  transform *= Matrix4::RotateY(yaw);
  transform *= Matrix4::RotateZ(roll);
  return transform;
}