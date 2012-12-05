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
  AffineTransform rotate_transform = position_.GetRotateMatrixFrom();
  AffineTransform transform = position_.GetTranslateMatrixFrom() * rotate_transform;
  // points
  positioned_points_.resize(model_->points().size());
  for (size_t i = 0; i < positioned_points_.size(); ++i) {
    positioned_points_[i] = transform * model_->points()[i];
  }

  // polygon normals
  positioned_polygon_normals_.resize(model_->polygon_normals().size());
  for (size_t i = 0; i < positioned_polygon_normals_.size(); ++i) {
    positioned_polygon_normals_[i] = rotate_transform * model_->polygon_normals()[i];
  }
}
