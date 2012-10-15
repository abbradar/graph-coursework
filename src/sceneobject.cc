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
    myfloat minx = numeric_limits<myfloat>::max(),
        miny = numeric_limits<myfloat>::max(),
        minz = numeric_limits<myfloat>::max();
    myfloat maxx = -numeric_limits<myfloat>::max(),
        maxy = -numeric_limits<myfloat>::max(),
        maxz = -numeric_limits<myfloat>::max();
    for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
      Point3D &v = points_[p.points[i]];
      if (v.x < minx) minx = v.x;
      else if (v.x > maxx) maxx = v.x;
      if (v.y < miny) miny = v.y;
      else if (v.y > maxy) maxy = v.y;
      if (v.z < minz) minz = v.z;
      else if (v.z > maxz) maxz = v.z;
    }
    const Point3D &v0 = points_[p.points[0]],
        &v1 = points_[p.points[1]],
        &v2 = points_[p.points[2]];
    const Point3D &n0 = vertex_normals_[p.points[0]],
        &n1 = vertex_normals_[p.points[1]],
        &n2 = vertex_normals_[p.points[2]];
    myfloat x = (minx + maxx) / 2,
        y = (miny + maxy) / 2,
        z = (minz + maxz) / 2;
    Point3D result;
    myfloat xa, xb, xc, ya, yb, yc, za, zb, zc;
    QuadraticInterpolation(v0.x, n0.x, v1.x, n1.x, v2.x, n2.x, xa, xb, xc);
    QuadraticInterpolation(v0.y, n0.y, v1.y, n1.y, v2.y, n2.y, ya, yb, yc);
    QuadraticInterpolation(v0.z, n0.z, v1.z, n1.z, v2.z, n2.z, za, zb, zc);
    result.x = QuadraticFunc(xa, xb, xc, x);
    result.y = QuadraticFunc(ya, yb, yc, y);
    result.z = QuadraticFunc(za, zb, zc, z);
    polygon_normals_.push_back(result);
  }
}
