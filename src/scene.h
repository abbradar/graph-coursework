#ifndef GRAPH_SCENE_H_
#define GRAPH_SCENE_H_

#include <list>
#include <memory>
#include <string>
#include "sdlobj/color.h"
#include "position.h"
#include "sceneobject.h"

class Scene {
 public:
  typedef std::list<std::shared_ptr<SceneObject>> SceneObjectList;

  Scene();

  inline SceneObjectList &objects() {
    return objects_;
  }

  inline const SceneObjectList &objects() const {
    return objects_;
  }

  inline myfloat ambient_light() const {
    return ambient_light_;
  }

  void set_ambient_light(const myfloat ambient_light);

 private:
  SceneObjectList objects_;
  myfloat ambient_light_;
};

#endif // GRAPH_SCENE_H_
