#ifndef GRAPH_SCENE_H_
#define GRAPH_SCENE_H_

#include <list>
#include <memory>
#include <string>
#include "sdlobj/color.h"
#include "sceneobject.h"

class Scene {
 public:
  typedef std::list<std::shared_ptr<SceneObject>> ObjectList;

  Scene();

  inline ObjectList &objects() {
    return objects_;
  }

  inline const ObjectList &objects() const {
    return objects_;
  }

  inline sdlobj::Color &plane_color() {
    return plane_color_;
  }

  void set_plane_color(const sdlobj::Color &plane_color);

 private:
  ObjectList objects_;
  sdlobj::Color plane_color_;
};

#endif // GRAPH_SCENE_H_
