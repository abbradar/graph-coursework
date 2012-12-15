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

void SceneObject::set_light_source(const std::shared_ptr<LightSource> &light_source) {
  light_source_ = light_source;
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

#if defined(GOURAUD_SHADING) || defined(PHONG_SHADING)
  // vertex normals
  positioned_vertex_normals_.resize(model_->vertex_normals().size());
  for (size_t i = 0; i < positioned_vertex_normals_.size(); ++i) {
    positioned_vertex_normals_[i] = rotate_transform * model_->vertex_normals()[i];
  }
#endif

#ifdef FLAT_SHADING
  // triangle centers
  positioned_centers_.resize(model_->polygons().size());
  for (size_t i = 0; i < positioned_centers_.size(); ++i) {
    const int *const &points = model_->polygons()[i].points;
    positioned_centers_[i] = (positioned_points_[points[0]] + positioned_points_[points[1]]
        + positioned_points_[points[2]]) / 3;
  }
#endif
}
