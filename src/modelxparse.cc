#include "common/debug.h"
#include "common/exception.h"
#include "matrix4.h"
#include "myfloat.h"
#include "materialxparse.h"
#include "point3dxparse.h"
#include "indexedtrianglexparse.h"
#include "point2dxparse.h"
#include "matrix4xparse.h"
#include "modelxparse.h"

using namespace std;
using namespace xparse;

namespace xparse {

Model LoadModel(istream &in, const string &name, const XFile &templates,
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
      transform *= LoadFromTransformMatrix(transform_node);
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
      transform *= LoadFromTransformMatrix(transform_node);
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

  Point3DVector points;
  points.reserve(mesh->data[0]->data().int_value);
  for (auto &xvec : *(mesh->data[1]->data().array_value)) {
    Point3D point = LoadFromVector(*(xvec.node_value));
    point = transform * point;
    points.push_back(point);
  }

  TriangleVector triangles;
  triangles.reserve(mesh->data[2]->data().int_value);
  for (auto &xpoly : *(mesh->data[3]->data().array_value)) {
    triangles.push_back(LoadFromMeshFace(*(xpoly.node_value)));
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

  Point3DVector normal_points;
  normal_points.reserve(normals->data[0]->data().int_value);
  for (auto &xvec : *(normals->data[1]->data().array_value)) {
    Point3D point = LoadFromVector(*(xvec.node_value));
    point = rotate_transform * point;
    normal_points.push_back(point);
  }

  XData *material_list = nullptr;
  for (auto &normals_i : mesh->nested_data) {
    Assert(normals_i.type() == XNestedData::kNode);
    XData *j = normals_i.data().node;
    if (j->template_id == "MeshMaterialList") {
      material_list = j;
      break;
    }
  }
  if (!material_list) throw Exception("MeshMaterialList not found");

  MaterialVector materials;
  shared_ptr<MaterialIndexVector> material_indexes;
  materials.reserve(material_list->data[0]->data().int_value);
  size_t mindexes_size = material_list->data[1]->data().int_value;
  if (mindexes_size >= triangles.size()) {
    material_indexes = make_shared<MaterialIndexVector>();
    for (auto &material : *(material_list->data[2]->data().array_value)) {
      material_indexes->push_back(material.int_value);
    }
  }

  for (auto &materials_i : material_list->nested_data) {
    Assert(materials_i.type() == XNestedData::kNode);
    XData *material = materials_i.data().node;
    if (material->template_id == "Material") {
      materials.push_back(LoadFromMaterial(material));
    }
  }

  Model object = Model(name);
  object.set_model(points, triangles, normal_points, materials, material_indexes);

  XData *texture_coords = nullptr;
  for (auto &uv_i : mesh->nested_data) {
    Assert(uv_i.type() == XNestedData::kNode);
    XData *j = uv_i.data().node;
    if (j->template_id == "MeshTextureCoords") {
      texture_coords = j;
      break;
    }
  }
  if (texture_coords) {
    std::shared_ptr<Point2DVector> uv_coords = make_shared<Point2DVector>();
    uv_coords->reserve(texture_coords->data[0]->data().int_value);
    for (auto &uv : *(texture_coords->data[1]->data().array_value)) {
      Point2D point = LoadFromCoords2D(*(uv.node_value));
      uv_coords->push_back(point);
    }
    object.set_uv_coords(uv_coords);
  }

  return object;
}

}
