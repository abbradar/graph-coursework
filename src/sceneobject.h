#ifndef GRAPH_SCENEOBJECT_H_
#define GRAPH_SCENEOBJECT_H_

#include <memory>
#include <yaml-cpp/yaml.h>
#include "position.h"
#include "models.h"
#include "model.h"
#include "config.h"
#include "lightsource.h"

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

  inline const std::shared_ptr<LightSource> &light_source() const {
    return light_source_;
  }

  void set_light_source(const std::shared_ptr<LightSource> &light_source);

 private:
  std::shared_ptr<Model> model_;
  Vector3Vector positioned_points_;
  Position position_;

  Vector3Vector positioned_polygon_normals_;

#if defined(GOURAUD_SHADING) || defined(PHONG_SHADING)
  Vector3Vector positioned_vertex_normals_;
#endif

#ifdef FLAT_SHADING
  Vector3Vector positioned_centers_;
#endif

  std::shared_ptr<LightSource> light_source_;

  void UpdatePositioned();
};

#endif // GRAPH_SCENEOBJECT_H_
