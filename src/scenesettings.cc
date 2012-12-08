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

  for (auto &mi : doc["light-sources"]) {
    LightSource *obj = new LightSource();
    mi >> *obj;
    scene.light_sources().push_back(shared_ptr<LightSource>(obj));
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
  emitter << YAML::Key << "light-sources" << YAML::Value;

  if (scene.light_sources().size() > 0) {
    emitter << YAML::BeginSeq;

    for (auto &i : scene.light_sources()) {
      emitter << *i;
    }

    emitter << YAML::EndSeq;
  }

  emitter << YAML::Key << "ambient-light" << YAML::Value << scene.ambient_light();
  emitter << YAML::EndMap;

  out << emitter.c_str();
}

}
