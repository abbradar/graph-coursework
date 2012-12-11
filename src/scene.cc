#include "scene.h"

using namespace sdlobj;

Scene::Scene() : ambient_light_(1) {}

void Scene::set_ambient_light(const myfloat ambient_light) {
  ambient_light_ = ambient_light;
}
