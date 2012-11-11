#include <limits>
#include <fstream>
#include "common/exception.h"
#include "common/debug.h"
#include "common/math.h"
#include "model.h"

using namespace std;

Model::Model(const string &name) : name_(name) {}

Model::~Model() = default;

void Model::set_model(const Point3DVector &points, const TriangleVector &polygons,
                      const Point3DVector &vertex_normals, const MaterialVector &materials,
                      const std::shared_ptr<MaterialIndexVector> &material_indexes) {
  if (points.size() != vertex_normals.size()) {
    throw Exception("Points and vertex normals numbers mismatch");
  }
  if (material_indexes) {
    if (polygons.size() != material_indexes->size()) {
      throw Exception("Points and material indexes numbers mismatch");
    }
  } else {
    if (materials.size() != 1) {
      throw Exception("There should be only one Material if without indexes");
    }
  }
  points_ = points;
  polygons_ = polygons;
  vertex_normals_ = vertex_normals;
  materials_ = materials;
  material_indexes_ = material_indexes;
  ComputePolygonNormals();
}

void Model::set_uv_coords(std::shared_ptr<Point2DVector> &uv_coords) {
  if (uv_coords->size() != points_.size()) {
    throw Exception("Points and UV coords numbers mismatch");
  }
  uv_coords_ = uv_coords;
}

void Model::ComputePolygonNormals() {
  polygon_normals_.clear();
  polygon_normals_.reserve(polygons_.size());
  for (const auto &p : polygons_) {
    Point3D &offset = points_[p.points[0]];
    Point3D a = points_[p.points[1]] - offset,
        b = points_[p.points[2]] - offset;
    Point3D result = Point3D::VectorMul(a, b);
    char sign = Sign(Point3D::ScalarMul(vertex_normals_[p.points[0]], result));
    if (sign < 0) {
      result.x = -result.x;
      result.y = -result.y;
      result.z = -result.z;
    }
    polygon_normals_.push_back(result);
  }
}
