#include "sceneobjectsettings.h"
#include "lightsourcesettings.h"
#include "colorsettings.h"
#include "positionsettings.h"
#include "scenesettings.h"

using namespace std;

namespace settings {

void LoadScene(Scene &scene, istream &in, const Models &models) {
  YAML::Parser parser(in);
  YAML::Node doc;
  parser.GetNextDocument(doc);

  for (auto &mi : doc["objects"]) {
    SceneObject *obj = new SceneObject(LoadSceneObject(mi, models));
    scene.objects().push_back(shared_ptr<SceneObject>(obj));
  }

  myfloat ambient_light;
  doc["ambient-light"] >> ambient_light;
  scene.set_ambient_light(ambient_light);
}

void SaveScene(Scene &scene, ostream &out) {
  YAML::Emitter emitter;
  emitter << YAML::BeginMap;
  emitter << YAML::Key << "objects" << YAML::Value << YAML::BeginSeq;

  for (auto &i : scene.objects()) {
    emitter << *i;
  }

  emitter << YAML::EndSeq;

  emitter << YAML::Key << "ambient-light" << YAML::Value << scene.ambient_light();
  emitter << YAML::EndMap;

  out << emitter.c_str();
}

}
