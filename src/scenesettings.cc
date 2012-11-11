#include "sceneobjectsettings.h"
#include "colorsettings.h"
#include "scenesettings.h"

using namespace std;

namespace settings {

void LoadScene(Scene &scene, istream &in, const Models &models) {
  YAML::Parser parser(in);
  YAML::Node doc;
  parser.GetNextDocument(doc);

  for (auto &mi : doc) {
    SceneObject *obj = new SceneObject(LoadSceneObject(mi, models));
    scene.objects().push_back(shared_ptr<SceneObject>(obj));
  }
}

void SaveScene(Scene &scene, ostream &out) {
  YAML::Emitter emitter;
  emitter << YAML::BeginSeq;

  for (auto &i : scene.objects()) {
    emitter << *i;
  }

  emitter << YAML::EndSeq;
  out << emitter.c_str();
}

SceneSettings::SceneSettings(Scene *scene) : scene_(scene) {}

SceneSettings::~SceneSettings() = default;

const std::string SceneSettings::kName = std::string("scene");

const std::string SceneSettings::name() {
  return kName;
}

void SceneSettings::set_scene(Scene *scene) {
  scene_ = scene;
}

void SceneSettings::operator <<(const YAML::Node &node) {
  sdlobj::Color plane_color;
  node["plane-color"] >> plane_color;
  scene_->set_plane_color(plane_color);
}

}
