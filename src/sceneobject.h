#ifndef GRAPH_SCENEOBJECT_H_
#define GRAPH_SCENEOBJECT_H_

#include <vector>
#include "polygon.h"

/** Scene object */
class SceneObject {
 public:
  SceneObject();

  /** Polygons vector */
  inline std::vector<Polygon> &polygons() {
    return polygons_;
  }

 private:
  std::vector<Polygon> polygons_;
};

#endif // GRAPH_SCENEOBJECT_H_
