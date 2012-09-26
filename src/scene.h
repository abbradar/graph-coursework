#ifndef GRAPH_SCENE_H_
#define GRAPH_SCENE_H_

#include <list>
#include "sceneobject.h"

struct Position {
  float x = 0;
  float y = 0;
  float z = 0;
  float pitch = 0;
  float yaw = 0;
  float roll = 0;
};

class Scene {
 public:
 private:
  std::list<SceneObject> objects_;
};

#endif // GRAPH_SCENE_H_
