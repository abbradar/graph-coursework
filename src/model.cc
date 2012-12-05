#include <limits>
#include <fstream>
#include "common/exception.h"
#include "common/debug.h"
#include "common/math.h"
#include "model.h"

using namespace std;

Model::Model(const string &name) : name_(name) {}

Model::~Model() = default;

void Model::set_model(const Vector3Vector &points, const TriangleVector &polygons,
                      const Vector3Vector &vertex_normals, const MaterialVector &materials) {
  if (points.size() != vertex_normals.size()) {
    throw Exception("Points and vertex normals numbers mismatch");
  }
  if (polygons.size() != materials.size()) {
    throw Exception("Points and materials numbers mismatch");
  }
  points_ = points;
  polygons_ = polygons;
  vertex_normals_ = vertex_normals;
  materials_ = materials;
  ComputePolygonNormals();
}

void Model::set_uv_coords(std::shared_ptr<Vector2Vector> &uv_coords) {
  if (uv_coords->size() != points_.size()) {
    throw Exception("Points and UV coords numbers mismatch");
  }
  uv_coords_ = uv_coords;
}

void Model::ComputePolygonNormals() {
  polygon_normals_.clear();
  polygon_normals_.reserve(polygons_.size());
  for (const auto &p : polygons_) {
    Vector3 &offset = points_[p.points[0]];
    Vector3 a = points_[p.points[1]] - offset,
        b = points_[p.points[2]] - offset;
    Vector3 result = a.cross(b);
    char sign = Sign(vertex_normals_[p.points[0]].dot(result));
    if (sign < 0) {
      result *= -1;
    }
    polygon_normals_.push_back(result);
  }
}
