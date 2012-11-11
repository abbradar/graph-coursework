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

typedef std::vector<Point3D> Point3DVector;
typedef std::vector<Point2D> Point2DVector;
typedef std::vector<IndexedTriangle> TriangleVector;
typedef std::vector<Material> MaterialVector;
typedef std::vector<size_t> MaterialIndexVector;

class Model {
 public:
  Model(const std::string &name);
  ~Model();

  inline const std::string &name() const {
    return name_;
  }

  /** Points vector */
  inline const Point3DVector &points() const {
    return points_;
  }

  inline const TriangleVector &polygons() const {
    return polygons_;
  }

  void set_model(const Point3DVector &points, const TriangleVector &polygons,
                 const Point3DVector &vertex_normals, const MaterialVector &materials,
                 const std::shared_ptr<MaterialIndexVector> &material_indexes);

  inline const Point3DVector &vertex_normals() const {
    return vertex_normals_;
  }

  inline const Point3DVector &polygon_normals() const {
    return polygon_normals_;
  }

  inline const MaterialVector &materials() const {
    return materials_;
  }

  inline const std::shared_ptr<MaterialIndexVector> &material_indexes() const {
    return material_indexes_;
  }

  inline const Point2DVector uv_coords() const {
    return *uv_coords_;
  }

  void set_uv_coords(std::shared_ptr<Point2DVector> &uv_coords);

 private:
  std::string name_;
  Point3DVector points_;
  TriangleVector polygons_;
  Point3DVector vertex_normals_;
  Point3DVector polygon_normals_;
  std::shared_ptr<Point2DVector> uv_coords_;
  MaterialVector materials_;
  std::shared_ptr<MaterialIndexVector> material_indexes_;

  void ComputePolygonNormals();
};

#endif // GRAPH_MODEL_H_
