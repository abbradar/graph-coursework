#include "common/math.h"
#include "common/debug.h"
#include "common/exception.h"
#include "matrix4.h"
#include "myfloat.h"
#include "materialxparse.h"
#include "vector3xparse.h"
#include "indexedtrianglexparse.h"
#include "vector2xparse.h"
#include "matrix4xparse.h"
#include "modelxparse.h"

using namespace std;
using namespace xparse;

namespace xparse {

Model LoadModel(istream &in, const string &name, const XFile &templates,
                const string &root_frame, const string &model_frame,
                const string &textures_path, const bool lighting_material) {
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

  AffineTransform full_transform(transform);
  transform(0, 3) = 0;
  transform(1, 3) = 0;
  transform(2, 3) = 0;
  AffineTransform rotate_transform(transform);

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

  Vector3Vector points;
  points.reserve(mesh->data[0]->data().int_value);
  for (auto &xvec : *(mesh->data[1]->data().array_value)) {
    Vector3 point = LoadFromVector(*(xvec.node_value));
    point = full_transform * point;
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

  Vector3Vector normal_points;
  normal_points.reserve(normals->data[0]->data().int_value);
  for (auto &xvec : *(normals->data[1]->data().array_value)) {
    Vector3 point = LoadFromVector(*(xvec.node_value));
    point = rotate_transform * point;
    normal_points.push_back(point.normalized());
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
  materials.reserve(material_list->data[0]->data().int_value);
  for (auto &materials_i : material_list->nested_data) {
    Assert(materials_i.type() == XNestedData::kNode);
    XData *material = materials_i.data().node;
    if (material->template_id == "Material") {
      auto newmat = make_shared<Material>(LoadFromMaterial(material, textures_path));
      newmat->set_lighting(lighting_material);
      materials.push_back(std::move(newmat));
    }
  }
  if (materials.size() == 0) throw Exception("Material not found");

  MaterialVector material_indexes;
  material_indexes.reserve(triangles.size());
  size_t mi_size = material_list->data[1]->data().int_value;
  if (mi_size < triangles.size()) {
    for (size_t i = 0; i < triangles.size(); ++i) {
      material_indexes.push_back(materials[0]);
    }
  } else {
    for (auto &material : *(material_list->data[2]->data().array_value)) {
      material_indexes.push_back(materials[material.int_value]);
    }
  }

  Model object = Model(name);
  object.set_model(points, triangles, normal_points, material_indexes);

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
    std::shared_ptr<Vector2Vector> uv_coords = make_shared<Vector2Vector>();
    uv_coords->reserve(texture_coords->data[0]->data().int_value);
    for (auto &uv : *(texture_coords->data[1]->data().array_value)) {
      Vector2 point = LoadFromCoords2D(*(uv.node_value));
      //if (!InBounds<myfloat>(point.x(), 0, 1) || !InBounds<myfloat>(point.y(), 0, 1)) {
      //  throw Exception("Invalid texture coordinates.");
      //}
      uv_coords->push_back(point);
    }
    object.set_uv_coords(uv_coords);
  }

  return object;
}

}
