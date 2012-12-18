#include <memory>
#include "common/exception.h"
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
  const YAML::Node *light = node.FindValue("light-source");

  SceneObject object(model->second, move(position));

  if (light) {
    std::shared_ptr<LightSource> source = make_shared<LightSource>();
    (*light)["specular"] >> source->specular;
    (*light)["diffuse"] >> source->diffuse;
    object.set_light_source(source);
  }

  return object;
}

void operator <<(YAML::Emitter &emitter, const SceneObject &scene_object) {
  emitter << YAML::BeginMap;
  emitter << YAML::Key << "model-name" << YAML::Value << scene_object.model()->name();
  emitter << YAML::Key << "position"  << YAML::Value << scene_object.position();
  if (scene_object.light_source()) {
    emitter << YAML::Key << "light-source" << YAML::Value << YAML::BeginMap;
    emitter << YAML::Key << "specular" << YAML::Value <<
               scene_object.light_source()->specular;
    emitter << YAML::Key << "diffuse" << YAML::Value <<
               scene_object.light_source()->diffuse;
    emitter << YAML::EndMap;
  }
  emitter << YAML::EndMap;
}

}
