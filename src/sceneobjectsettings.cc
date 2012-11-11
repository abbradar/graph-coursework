#include <memory>
#include "positionsettings.h"
#include "sceneobjectsettings.h"

using namespace std;

namespace settings {

SceneObject LoadSceneObject(const YAML::Node &node, const Models &models) {
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

}
