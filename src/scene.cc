#include <fstream>
#include "common/exception.h"
#include "common/logging.h"
#include "common/debug.h"
#include "common/math.h"
#include "xparse/xfile.h"
#include "colorsettings.h"
#include "scene.h"

using namespace std;
using namespace sdlobj;
using namespace xparse;

Scene::Scene() = default;

void Scene::set_plane_color(const Color &plane_color) {
  plane_color_ = plane_color;
}

bool Scene::Load(std::istream &in, const std::string root_frame) {
  if (!xfile_.Parse(in)) return false;

  XData *root = nullptr;
  for (auto &i : xfile_.data_nodes()) {
    if (i->id == root_frame && i->template_id == "Frame") {
      root = i.get();
      break;
    }
  }
  if (!root) return false;

  Matrix4 root_transform = Matrix4::Identity();
  for (auto &transform_i : root->nested_data) {
    Assert(transform_i.type() == XNestedData::kNode);
    XData *transform_node = transform_i.data().node;
    if (transform_node->template_id == "FrameTransformMatrix") {
      root_transform *= Matrix4::LoadFromXTransformMatrix(transform_node);
      break;
    }
  }

  for (auto &i : root->nested_data) {
    Assert(i.type() == XNestedData::kNode);
    XData *node = i.data().node;
    if (node->template_id == "Frame") {
      SceneObject object = SceneObject::LoadFromFrame(node, root_transform);
      objects_.push_back(object);
    }
  }

  return true;
}

bool Scene::LoadTemplates(std::istream &in) {
  return xfile_.Parse(in);
}

SceneSettings::SceneSettings(Scene *scene) : scene_(scene) {}

SceneSettings::~SceneSettings() = default;

const std::string SceneSettings::kName = std::string("scene");

const std::string SceneSettings::name() {
  return kName;
}

void SceneSettings::set_scene(Scene *scene) {
  scene_ = scene;
}

void SceneSettings::operator <<(const YAML::Node &node) {
  sdlobj::Color plane_color;
  node["plane-color"] >> plane_color;
  scene_->set_plane_color(plane_color);
  std::string x_templates_name;
  node["templates"] >> x_templates_name;
  std::ifstream x_templates(x_templates_name);
  if (!scene_->LoadTemplates(x_templates)) {
    throw Exception("Cannot load .x templates.");
  }
  x_templates.close();
  std::string root_frame;
  node["root-frame"] >> root_frame;
  std::string x_models_name;
  node["models"] >> x_models_name;
  std::ifstream x_models(x_models_name);
  if (!scene_->Load(x_models, root_frame)) {
    throw Exception("Cannot load .x models.");
  }
  x_models.close();
}
