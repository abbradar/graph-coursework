#ifndef GRAPH_MODEL_H_
#define GRAPH_MODEL_H_

#include <memory>
#include <string>
#include <vector>
#include <istream>
#include "sdlobj/color.h"
#include "point3d.h"
#include "point2d.h"
#include "material.h"
#include "indexedtriangle.h"
#include "matrix4.h"

typedef std::vector<Vector2> Vector2Vector;
typedef std::vector<Vector3> Vector3Vector;
typedef std::vector<IndexedTriangle> TriangleVector;
typedef std::vector<std::shared_ptr<Material>> MaterialVector;

class Model {
 public:
  Model(const std::string &name);
  ~Model();

  inline const std::string &name() const {
    return name_;
  }

  /** Points vector */
  inline const Vector3Vector &points() const {
    return points_;
  }

  inline const TriangleVector &polygons() const {
    return polygons_;
  }

  void set_model(const Vector3Vector &points, const TriangleVector &polygons,
                 const Vector3Vector &vertex_normals,
                 const MaterialVector &material_indexes);

  inline const Vector3Vector &vertex_normals() const {
    return vertex_normals_;
  }

  inline const Vector3Vector &polygon_normals() const {
    return polygon_normals_;
  }

  inline MaterialVector &materials() {
    return materials_;
  }

  inline const std::shared_ptr<Vector2Vector> &uv_coords() const {
    return uv_coords_;
  }

  void set_uv_coords(std::shared_ptr<Vector2Vector> &uv_coords);

 private:
  std::string name_;
  Vector3Vector points_;
  TriangleVector polygons_;
  Vector3Vector vertex_normals_;
  Vector3Vector polygon_normals_;
  MaterialVector materials_;
  std::shared_ptr<Vector2Vector> uv_coords_;

  void ComputePolygonNormals();
};

#endif // GRAPH_MODEL_H_
