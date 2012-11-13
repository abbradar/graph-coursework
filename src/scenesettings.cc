#include "sceneobjectsettings.h"
#include "colorsettings.h"
#include "positionsettings.h"
#include "scenesettings.h"

using namespace std;

namespace settings {

void LoadScene(Scene &scene, istream &in, const Models &models) {
  YAML::Parser parser(in);
  YAML::Node doc;
  parser.GetNextDocument(doc);

  doc["position"] >> *scene.position();
  for (auto &mi : doc["objects"]) {
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

}
