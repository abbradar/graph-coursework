#include "position.h"

Position::Position() : Position(0, 0, 0, 0, 0, 0) {}

Position::Position(myfloat x_, myfloat y_, myfloat z_, myfloat roll_, myfloat pitch_, myfloat yaw_)
  : x(x_), y(y_), z(z_), roll(roll_), pitch(pitch_), yaw(yaw_) {}

Matrix4 Position::GetMatrixFrom() {
  Matrix4 transform = GetTranslateMatrixFrom();
  transform *= GetRotateMatrixFrom();
  return transform;
}

Matrix4 Position::GetMatrixTo() {
  Matrix4 transform = GetRotateMatrixTo();
  transform *= GetTranslateMatrixTo();
  return transform;
}

Matrix4 Position::GetRotateMatrixFrom() {
  Matrix4 transform = Matrix4::RotateZ(yaw);
  transform *= Matrix4::RotateY(pitch);
  transform *= Matrix4::RotateX(roll);
  return transform;
}

Matrix4 Position::GetRotateMatrixTo() {
  Matrix4 transform = Matrix4::RotateX(-roll);
  transform *= Matrix4::RotateY(-pitch);
  transform *= Matrix4::RotateZ(-yaw);
  return transform;
}

Matrix4 Position::GetTranslateMatrixFrom() {
  return Matrix4::Translate(x, y, z);
}

Matrix4 Position::GetTranslateMatrixTo() {
  return Matrix4::Translate(-x, -y, -z);
}

void operator >>(const YAML::Node &node, Position &position) {
  node[0] >> position.x;
  node[1] >> position.y;
  node[2] >> position.z;
  node[3] >> position.roll;
  node[4] >> position.pitch;
  node[5] >> position.yaw;
}

void operator <<(YAML::Emitter &emitter, const Position &position) {
  emitter << YAML::Flow << YAML::BeginSeq;
  emitter << position.x << position.y << position.z;
  emitter << position.roll << position.pitch << position.yaw;
  emitter << YAML::EndSeq;
}
