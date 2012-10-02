#include "position.h"

Position::Position() : Position(0, 0, 0, 0, 0, 0) {}

Position::Position(myfloat x_, myfloat y_, myfloat z_, myfloat roll_, myfloat pitch_, myfloat yaw_)
  : x(x_), y(y_), z(z_), roll(roll_), pitch(pitch_), yaw(yaw_) {}

Matrix4 Position::GetMatrixFrom() {
  Matrix4 transform = Matrix4::Translate(x, y, z);
  transform *= Matrix4::RotateZ(yaw);
  transform *= Matrix4::RotateY(pitch);
  transform *= Matrix4::RotateX(roll);
  return transform;
}

Matrix4 Position::GetMatrixTo() {
  Matrix4 transform = Matrix4::RotateX(-roll);
  transform *= Matrix4::RotateY(-pitch);
  transform *= Matrix4::RotateZ(-yaw);
  transform *= Matrix4::Translate(-x, -y, -z);
  return transform;
}
