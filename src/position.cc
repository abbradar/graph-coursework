#include "position.h"

Position::Position() : Position(0, 0, 0, 0, 0, 0) {}

Position::Position(my_float x_, my_float y_, my_float z_, my_float pitch_, my_float yaw_, my_float roll_)
  : x(x_), y(y_), z(z_), pitch(pitch_), yaw(yaw_), roll(roll_) {}

Matrix4 Position::GetMatrixFrom() {
  Matrix4 transform = Matrix4::Translate(x, y, z);
  transform *= Matrix4::RotateZ(roll);
  transform *= Matrix4::RotateY(yaw);
  transform *= Matrix4::RotateX(pitch);
  return transform;
}

Matrix4 Position::GetMatrixTo() {
  Matrix4 transform = Matrix4::RotateX(-pitch);
  transform *= Matrix4::RotateY(-yaw);
  transform *= Matrix4::RotateZ(-roll);
  transform *= Matrix4::Translate(-x, -y, -z);
  return transform;
}
