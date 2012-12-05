#ifndef GRAPH_SCENEOBJECT_H_
#define GRAPH_SCENEOBJECT_H_

#include <memory>
#include <yaml-cpp/yaml.h>
#include "position.h"
#include "models.h"
#include "model.h"

/** Scene object */
class SceneObject {
 public:
  SceneObject();
  SceneObject(const std::shared_ptr<Model> &model, const Position &position);

  /** Points vector */
  inline const std::shared_ptr<Model> &model() const {
    return model_;
  }

  void set_model(const std::shared_ptr<Model> &model);

  inline const Vector3Vector &positioned_points() const {
    return positioned_points_;
  }

  inline const Vector3Vector &positioned_polygon_normals() const {
    return positioned_polygon_normals_;
  }

  inline const Position &position() const {
    return position_;
  }

  void set_position(const Position &position);

 private:
  std::shared_ptr<Model> model_;
  Vector3Vector positioned_points_;
  Vector3Vector positioned_polygon_normals_;
  Position position_;

  void UpdatePositioned();
};

#endif // GRAPH_SCENEOBJECT_H_
