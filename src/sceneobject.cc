#include <limits>
#include "common/math.h"
#include "matrix4.h"
#include "sceneobject.h"

using namespace std;

SceneObject::SceneObject() = default;

SceneObject::SceneObject(const PointVector &points, const TriangleVector &polygons,
                         const PointVector &vertex_normals, const Position &position)
 : points_(points), polygons_(polygons), vertex_normals_(vertex_normals), position_(position) {
  ComputePolygonNormals();
  UpdatePositioned();
}

void SceneObject::set_model(const PointVector &points, const TriangleVector &polygons,
                            const PointVector &vertex_normals) {
  points_ = points;
  polygons_ = polygons;
  vertex_normals_ = vertex_normals;
  ComputePolygonNormals();
  UpdatePositioned();
}

void SceneObject::set_position(const Position &position) {
  position_ = position;
  UpdatePositioned();
}

void SceneObject::UpdatePositioned() {
  Matrix4 rotate_transform = position_.GetRotateMatrixFrom();
  Matrix4 transform = position_.GetTranslateMatrixFrom() * rotate_transform;

  // points
  positioned_points_.clear();
  positioned_points_.reserve(points_.size());
  for (const auto &i : points_) {
    positioned_points_.push_back(transform * i);
  }

  // polygon normals
  positioned_polygon_normals_.clear();
  positioned_polygon_normals_.reserve(polygons_.size());
  for (const auto &i : polygon_normals_) {
    positioned_polygon_normals_.push_back(rotate_transform * i);
  }
}

void SceneObject::ComputePolygonNormals() {
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
