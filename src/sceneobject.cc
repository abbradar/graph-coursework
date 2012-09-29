#include "matrix4.h"
#include "sceneobject.h"

SceneObject::SceneObject() = default;

SceneObject::SceneObject(const SceneObject::PointVector &points, const Position &position)
 : points_(points), position_(position) {
  UpdatePositioned();
}

void SceneObject::set_model(const SceneObject::PointVector &points, const SceneObject::PolygonVector &polygons) {
  points_ = points;
  polygons_ = polygons;
  UpdatePositioned();
  ClearTransformed();
}

void SceneObject::ApplyTransform(const Matrix4 &transform) {
  if (transformed_points_.empty()) {
    transformed_points_ = positioned_points_;
  }
  for (auto &i : transformed_points_) {
    i = transform * i;
  }
}

void SceneObject::ClearTransformed() {
  transformed_points_.clear();
}

bool SceneObject::ClearedTransformed() {
  return transformed_points_.empty();
}

void SceneObject::set_position(const Position &position) {
  position_ = position;
  UpdatePositioned();
  ClearTransformed();
}

void SceneObject::UpdatePositioned() {
  positioned_points_.clear();
  positioned_points_.reserve(points_.size());
  Matrix4 transform = position_.GetTransformMatrix();
  for (const auto &i : points_) {
    positioned_points_.push_back(transform * i);
  }
}
