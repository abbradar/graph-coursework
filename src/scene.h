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
  typedef std::list<std::shared_ptr<SceneObject>> ObjectList;

  Scene();

  inline ObjectList &objects() {
    return objects_;
  }

  inline const ObjectList &objects() const {
    return objects_;
  }

  inline const std::shared_ptr<Position> &position() const {
    return position_;
  }

 private:
  ObjectList objects_;
  std::shared_ptr<Position> position_;
};

#endif // GRAPH_SCENE_H_
