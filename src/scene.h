#ifndef GRAPH_SCENE_H_
#define GRAPH_SCENE_H_

#include <list>
#include "sdlobj/color.h"
#include "sceneobject.h"

class Scene {
 public:
  Scene();

  inline std::list<SceneObject> &objects() {
    return objects_;
  }

  inline sdlobj::Color &plane_color() {
    return plane_color_;
  }

  void set_plane_color(const sdlobj::Color &plane_color);

  void Load();

 private:
  std::list<SceneObject> objects_;
  sdlobj::Color plane_color_;
};

#endif // GRAPH_SCENE_H_
