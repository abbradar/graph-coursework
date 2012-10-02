#include "matrix4.h"
#include "sceneobject.h"

SceneObject::SceneObject() = default;

SceneObject::SceneObject(const PointVector &points, const TriangleVector &polygons, const Position &position)
 : points_(points), polygons_(polygons), position_(position) {
  UpdatePositioned();
}

void SceneObject::set_model(const PointVector &points, const TriangleVector &polygons) {
  points_ = points;
  polygons_ = polygons;
  UpdatePositioned();
}

void SceneObject::set_position(const Position &position) {
  position_ = position;
  UpdatePositioned();
}

void SceneObject::UpdatePositioned() {
  positioned_points_.clear();
  positioned_points_.reserve(points_.size());
  Matrix4 transform = position_.GetMatrixFrom();
  for (const auto &i : points_) {
    positioned_points_.push_back(transform * i);
  }
}
