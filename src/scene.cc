#include "scene.h"

using namespace sdlobj;

Scene::Scene() = default;

void Scene::set_plane_color(const Color &plane_color) {
  plane_color_ = plane_color;
}
