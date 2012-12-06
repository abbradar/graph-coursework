#include "scene.h"

using namespace sdlobj;

Scene::Scene() : position_(new Position()) {}

void Scene::set_ambient_light(const myfloat ambient_light) {
  ambient_light_ = ambient_light;
}
