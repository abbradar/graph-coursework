#include <limits>
#include "common/exception.h"
#include "common/debug.h"
#include "common/math.h"
#include "matrix4.h"
#include "sceneobject.h"

using namespace std;
using namespace xparse;

SceneObject::SceneObject() = default;

SceneObject::SceneObject(const PointVector &points, const TriangleVector &polygons,
                         const PointVector &vertex_normals, const Position &position)
 : points_(points), polygons_(polygons), vertex_normals_(vertex_normals), position_(position) {
  ComputePolygonNormals();
  UpdatePositioned();
}

SceneObject SceneObject::LoadFromFrame(XData *frame, const Matrix4 &transform) {
  static const Point3D camera_direction = Point3D(1, 0, 0);

  if (frame->template_id != "Frame") {
    throw Exception("Invalid frame data");
  }

  Matrix4 frame_transform = transform;
  Position position;
  for (auto &transform_i : frame->nested_data) {
    Assert(transform_i.type() == XNestedData::kNode);
    XData *transform_node = transform_i.data().node;
    if (transform_node->template_id == "FrameTransformMatrix") {
      frame_transform *= Matrix4::LoadFromXTransformMatrix(transform_node);
      position.x = frame_transform.at(0, 3);
      frame_transform.at(0, 3) = 0;
      position.y = frame_transform.at(1, 3);
      frame_transform.at(1, 3) = 0;
      position.z = frame_transform.at(2, 3);
      frame_transform.at(2, 3) = 0;
    }
  }
  for (auto &mesh_i : frame->nested_data) {
    Assert(mesh_i.type() == XNestedData::kNode);
    XData *mesh = mesh_i.data().node;
    if (mesh->template_id == "Mesh") {
      PointVector points;
      points.reserve(mesh->data[0]->data().int_value);
      for (auto &xvec : *(mesh->data[1]->data().array_value)) {
        Point3D point = Point3D::LoadFromXVector(*(xvec.node_value));
        point = frame_transform * point;
        points.push_back(point);
      }
      TriangleVector triangles;
      triangles.reserve(mesh->data[2]->data().int_value);
      for (auto &xpoly : *(mesh->data[3]->data().array_value)) {
        triangles.push_back(IndexedTriangle::LoadFromXMeshFace(*(xpoly.node_value)));
      }
      for (auto &normals_i : mesh->nested_data) {
        Assert(normals_i.type() == XNestedData::kNode);
        XData *normals = normals_i.data().node;
        if (normals->template_id == "MeshNormals") {
          PointVector normal_points;
          normal_points.reserve(normals->data[0]->data().int_value);
          for (auto &xvec : *(normals->data[1]->data().array_value)) {
            Point3D point = Point3D::LoadFromXVector(*(xvec.node_value));
            point = frame_transform * point;
            normal_points.push_back(point);
          }

          SceneObject object = SceneObject(points, triangles, normal_points, Position());
          object.set_position(position);
          object.name() = frame->id;
          // TODO: remove this
          object.color() = sdlobj::Color(0, 255, 0);
          return object;
        }
        throw Exception("Normals data not found");
      }
    }
  }

  throw Exception("Invalid frame data");
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
