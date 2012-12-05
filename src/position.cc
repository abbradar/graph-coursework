#include <Eigen/Geometry>
#include "position.h"

const Vector3 Position::kCameraDirection = Vector3::UnitX();

Position::Position() : Position(0, 0, 0, 0, 0, 0) {}

Position::Position(myfloat x_, myfloat y_, myfloat z_, myfloat roll_, myfloat pitch_, myfloat yaw_)
  : x(x_), y(y_), z(z_), roll(roll_), pitch(pitch_), yaw(yaw_) {}

AffineTransform Position::GetMatrixFrom() const {
  AffineTransform transform = GetTranslateMatrixFrom() * GetRotateMatrixFrom();
  return transform;
}

AffineTransform Position::GetMatrixTo() const {
  AffineTransform transform = GetRotateMatrixTo() * GetTranslateMatrixTo();
  return transform;
}

AffineTransform Position::GetRotateMatrixFrom() const {
  AffineTransform transform(
        RotateTransform(yaw, Vector3::UnitZ())
      * RotateTransform(pitch, Vector3::UnitY())
      * RotateTransform(roll, Vector3::UnitX()));
  return transform;
}

AffineTransform Position::GetRotateMatrixTo() const {
  AffineTransform transform(
        RotateTransform(-roll, Vector3::UnitX())
      * RotateTransform(-pitch, Vector3::UnitY())
      * RotateTransform(-yaw, Vector3::UnitZ()));
  return transform;
}

AffineTransform Position::GetTranslateMatrixFrom() const {
  return AffineTransform(TranslateTransform(x, y, z));
}

AffineTransform Position::GetTranslateMatrixTo() const {
  return AffineTransform(TranslateTransform(-x, -y, -z));
}

Vector3 Position::GetVector3() const {
  return Vector3(x, y, z);
}
