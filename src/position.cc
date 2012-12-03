#include "position.h"

const Point3D Position::kCameraDirection = Point3D(1, 0, 0);

Position::Position() : Position(0, 0, 0, 0, 0, 0) {}

Position::Position(myfloat x_, myfloat y_, myfloat z_, myfloat roll_, myfloat pitch_, myfloat yaw_)
  : x(x_), y(y_), z(z_), roll(roll_), pitch(pitch_), yaw(yaw_) {}

Matrix4 Position::GetMatrixFrom() const {
  Matrix4 transform = GetTranslateMatrixFrom();
  transform *= GetRotateMatrixFrom();
  return transform;
}

Matrix4 Position::GetMatrixTo() const {
  Matrix4 transform = GetRotateMatrixTo();
  transform *= GetTranslateMatrixTo();
  return transform;
}

Matrix4 Position::GetRotateMatrixFrom() const {
  Matrix4 transform = Matrix4::RotateZ(yaw);
  transform *= Matrix4::RotateY(pitch);
  transform *= Matrix4::RotateX(roll);
  return transform;
}

Matrix4 Position::GetRotateMatrixTo() const {
  Matrix4 transform = Matrix4::RotateX(-roll);
  transform *= Matrix4::RotateY(-pitch);
  transform *= Matrix4::RotateZ(-yaw);
  return transform;
}

Matrix4 Position::GetTranslateMatrixFrom() const {
  return Matrix4::Translate(x, y, z);
}

Matrix4 Position::GetTranslateMatrixTo() const {
  return Matrix4::Translate(-x, -y, -z);
}

Point3D Position::GetPoint3D() const {
  return Point3D(x, y, z);
}
