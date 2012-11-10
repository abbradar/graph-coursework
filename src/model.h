#ifndef GRAPH_MODEL_H_
#define GRAPH_MODEL_H_

#include <string>
#include <vector>
#include <istream>
#include <yaml-cpp/yaml.h>
#include "xparse/xdata.h"
#include "sdlobj/color.h"
#include "point3d.h"
#include "indexedtriangle.h"
#include "matrix4.h"

typedef std::vector<Point3D> PointVector;
typedef std::vector<IndexedTriangle> TriangleVector;

class Model {
 public:
  Model(const std::string &name);
  Model(const std::string &name, const PointVector &points, const TriangleVector &polygons,
        const PointVector &vertex_normals);
  ~Model();

  static Model LoadFromFrame(const std::string &name, std::istream &in, const xparse::XFile &templates,
                             const std::string &root_frame, const std::string &model_frame);
  static Model LoadFromYaml(const YAML::Node &node, const xparse::XFile &templates);

  inline const std::string &name() const {
    return name_;
  }

  /** Points vector */
  inline const PointVector &points() const {
    return points_;
  }

  inline const TriangleVector &polygons() const {
    return polygons_;
  }

  void set_model(const PointVector &points, const TriangleVector &polygons,
                 const PointVector &vertex_normals);

  inline const PointVector &vertex_normals() const {
    return vertex_normals_;
  }

  inline const PointVector &polygon_normals() const {
    return polygon_normals_;
  }

  inline sdlobj::Color &color() {
    return color_;
  }

 private:
  std::string name_;
  PointVector points_;
  TriangleVector polygons_;
  PointVector vertex_normals_;
  PointVector polygon_normals_;
  sdlobj::Color color_;

  void ComputePolygonNormals();
};

#endif // GRAPH_MODEL_H_
