#include <fstream>
#include <yaml-cpp/yaml.h>
#include "common/exception.h"
#include "common/logging.h"
#include "common/debug.h"
#include "common/math.h"
#include "xparse/xfile.h"
#include "colorsettings.h"
#include "scene.h"

using namespace std;
using namespace sdlobj;
using namespace xparse;

Scene::Scene() = default;

void Scene::set_plane_color(const Color &plane_color) {
  plane_color_ = plane_color;
}

void Scene::Load(istream &in, const Models &models) {
  YAML::Parser parser(in);
  YAML::Node doc;
  parser.GetNextDocument(doc);

  for (auto &mi : doc) {
    SceneObject *obj = new SceneObject(SceneObject::LoadFromYaml(mi, models));
    objects_.push_back(shared_ptr<SceneObject>(obj));
  }
}

void Scene::Save(ostream &out) {
  YAML::Emitter emitter;
  emitter << YAML::BeginSeq;

  for (auto &i : objects_) {
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
