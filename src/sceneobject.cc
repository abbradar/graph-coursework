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

SceneObject SceneObject::LoadFromYaml(const YAML::Node &node, const Models &models) {
  std::string model_name;
  node["model-name"] >> model_name;
  auto model = models.models().find(model_name);
  if (model == models.models().end()) {
    throw Exception("Invalid model name");
  }
  Position position;
  node["position"] >> position;
  return SceneObject(model->second, move(position));
}

void operator <<(YAML::Emitter &emitter, const SceneObject &scene_object) {
  emitter << YAML::BeginMap;
  emitter << YAML::Key << "model-name" << YAML::Value << scene_object.model()->name();
  emitter << YAML::Key << "position"  << YAML::Value << scene_object.position();
  emitter << YAML::EndMap;
}
