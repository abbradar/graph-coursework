#ifndef GRAPH_SCENE_H_
#define GRAPH_SCENE_H_

#include <list>
#include <memory>
#include <string>
#include "sdlobj/color.h"
#include "position.h"
#include "lightsource.h"
#include "sceneobject.h"

class Scene {
 public:
  typedef std::list<std::shared_ptr<SceneObject>> SceneObjectList;
  typedef std::vector<LightSource> LightSourceVector;

  Scene();

  inline SceneObjectList &objects() {
    return objects_;
  }

  inline const SceneObjectList &objects() const {
    return objects_;
  }

  inline LightSourceVector &light_sources() {
    return light_sources_;
  }

  inline const LightSourceVector &light_sources() const {
    return light_sources_;
  }

  inline myfloat ambient_light() const {
    return ambient_light_;
  }

  void set_ambient_light(const myfloat ambient_light);

 private:
  SceneObjectList objects_;
  LightSourceVector light_sources_;
  myfloat ambient_light_;
};

#endif // GRAPH_SCENE_H_
