#include <limits>
#include <fstream>
#include "common/exception.h"
#include "common/debug.h"
#include "common/math.h"
#include "model.h"

using namespace std;
using namespace xparse;

Model::Model(const string &name) : name_(name) {}

Model::Model(const string &name, const PointVector &points, const TriangleVector &polygons,
             const PointVector &vertex_normals)
 : name_(name), points_(points), polygons_(polygons), vertex_normals_(vertex_normals) {
  ComputePolygonNormals();
}

Model::~Model() = default;

Model Model::LoadFromFrame(const string &name, istream &in, const XFile &templates,
                           const string &root_frame, const string &model_frame) {
  static const myfloat system_transform_m[] = {1, 0, 0, 0,
                                               0, 0, 1, 0,
                                               0, 1, 0, 0,
                                               0, 0, 0, 1};

  XFile xfile = templates;
  if (!xfile.Parse(in)) throw Exception("Invalid .x file");

  XData *root = nullptr;
  for (auto &i : xfile.data_nodes()) {
    if (i->id == root_frame && i->template_id == "Frame") {
      root = i.get();
      break;
    }
  }
  if (!root) throw Exception("Root frame not found");

  Matrix4 transform(system_transform_m);
  for (auto &transform_i : root->nested_data) {
    Assert(transform_i.type() == XNestedData::kNode);
    XData *transform_node = transform_i.data().node;
    if (transform_node->template_id == "FrameTransformMatrix") {
      transform *= Matrix4::LoadFromXTransformMatrix(transform_node);
      break;
    }
  }

  XData *model = nullptr;
  for (auto &i : root->nested_data) {
    Assert(i.type() == XNestedData::kNode);
    XData *j = i.data().node;
    if (j->id == model_frame && j->template_id == "Frame") {
      model = j;
      break;
    }
  }
  if (!model) throw Exception("Model frame not found");

  for (auto &transform_i : model->nested_data) {
    Assert(transform_i.type() == XNestedData::kNode);
    XData *transform_node = transform_i.data().node;
    if (transform_node->template_id == "FrameTransformMatrix") {
      transform *= Matrix4::LoadFromXTransformMatrix(transform_node);
    }
  }

  Matrix4 rotate_transform = transform;
  for (size_t i = 0; i < Matrix4::kMatrixHeight - 1; ++i) {
    rotate_transform.at(Matrix4::kMatrixWidth - 1, i);
  }

  XData *mesh = nullptr;
  for (auto &mesh_i : model->nested_data) {
    Assert(mesh_i.type() == XNestedData::kNode);
    XData *j = mesh_i.data().node;
    if (j->template_id == "Mesh") {
      mesh = j;
      break;
    }
  }
  if (!mesh) throw Exception("Mesh not found");

  PointVector points;
  points.reserve(mesh->data[0]->data().int_value);
  for (auto &xvec : *(mesh->data[1]->data().array_value)) {
    Point3D point = Point3D::LoadFromXVector(*(xvec.node_value));
    point = transform * point;
    points.push_back(point);
  }

  TriangleVector triangles;
  triangles.reserve(mesh->data[2]->data().int_value);
  for (auto &xpoly : *(mesh->data[3]->data().array_value)) {
    triangles.push_back(IndexedTriangle::LoadFromXMeshFace(*(xpoly.node_value)));
  }

  XData *normals = nullptr;
  for (auto &normals_i : mesh->nested_data) {
    Assert(normals_i.type() == XNestedData::kNode);
    XData *j = normals_i.data().node;
    if (j->template_id == "MeshNormals") {
      normals = j;
      break;
    }
  }
  if (!normals) throw Exception("MeshNormals not found");

  PointVector normal_points;
  normal_points.reserve(normals->data[0]->data().int_value);
  for (auto &xvec : *(normals->data[1]->data().array_value)) {
    Point3D point = Point3D::LoadFromXVector(*(xvec.node_value));
    point = rotate_transform * point;
    normal_points.push_back(point);
  }

  Model object = Model(name, points, triangles, normal_points);
  // TODO: remove this
  object.color() = sdlobj::Color(0, 255, 0);
  return object;
}

Model Model::LoadFromYaml(const YAML::Node &node, const XFile &templates) {
  std::string name;
  node["name"] >> name;
  std::string root_frame;
  node["root-frame"] >> root_frame;
  std::string model_frame;
  node["model-frame"] >> model_frame;
  std::string x_model;
  node["model"] >> x_model;
  std::ifstream in(x_model);
  return Model::LoadFromFrame(name, in, templates, root_frame, model_frame);
}

void Model::set_model(const PointVector &points, const TriangleVector &polygons,
                      const PointVector &vertex_normals) {
  points_ = points;
  polygons_ = polygons;
  vertex_normals_ = vertex_normals;
  ComputePolygonNormals();
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
