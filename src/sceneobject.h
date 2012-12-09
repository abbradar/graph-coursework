#ifndef GRAPH_SCENEOBJECT_H_
#define GRAPH_SCENEOBJECT_H_

#include <memory>
#include <yaml-cpp/yaml.h>
#include "position.h"
#include "models.h"
#include "model.h"
#include "config.h"

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

#if !defined(NO_NORMAL_FACE_CLIPPING) || defined(FLAT_SHADING)
  inline const Vector3Vector &positioned_polygon_normals() const {
    return positioned_polygon_normals_;
  }
#endif

#if defined(GOURAUD_SHADING) || defined(PHONG_SHADING)
  inline const Vector3Vector &positioned_vertex_normals() const {
    return positioned_vertex_normals_;
  }
#endif

#ifdef FLAT_SHADING
  inline const Vector3Vector &positioned_centers() const {
    return positioned_centers_;
  }
#endif

  inline const Position &position() const {
    return position_;
  }

  void set_position(const Position &position);

 private:
  std::shared_ptr<Model> model_;
  Vector3Vector positioned_points_;
  Position position_;

#if !defined(NO_NORMAL_FACE_CLIPPING) || defined(FLAT_SHADING)
  Vector3Vector positioned_polygon_normals_;
#endif

#if defined(GOURAUD_SHADING) || defined(PHONG_SHADING)
  Vector3Vector positioned_vertex_normals_;
#endif

#ifdef FLAT_SHADING
  Vector3Vector positioned_centers_;
#endif

  void UpdatePositioned();
};

#endif // GRAPH_SCENEOBJECT_H_
