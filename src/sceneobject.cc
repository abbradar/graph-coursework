#include "common/exception.h"
#include "sceneobject.h"

using namespace std;

SceneObject::SceneObject() = default;

SceneObject::SceneObject(const shared_ptr<Model> &model, const Position &position) :
 model_(), position_(position) {
  set_model(model);
}

void SceneObject::set_model(const shared_ptr<Model> &model) {
  model_ = model;
  UpdatePositioned();
}

void SceneObject::set_position(const Position &position) {
  position_ = position;
  UpdatePositioned();
}

void SceneObject::UpdatePositioned() {
  Matrix4 rotate_transform = position_.GetRotateMatrixFrom();
  Matrix4 transform = position_.GetTranslateMatrixFrom() * rotate_transform;
  // points
  positioned_points_.clear();
  positioned_points_.reserve(model_->points().size());
  for (const auto &i : model_->points()) {
    positioned_points_.push_back(transform * i);
  }

  // polygon normals
  positioned_polygon_normals_.clear();
  positioned_polygon_normals_.reserve(model_->polygon_normals().size());
  for (const auto &i : model_->polygon_normals()) {
    positioned_polygon_normals_.push_back(rotate_transform * i);
  }
}
