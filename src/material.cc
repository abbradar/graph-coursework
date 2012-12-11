#include "common/exception.h"
#include "material.h"

#ifndef NO_LIGHTING
#include "surfacepainterfactory.h"
#include "colortransforms.h"
#endif

using namespace sdlobj;
using namespace std;

Material::Material() : shininess_(0), ambient_color_(1, 1, 1), specular_color_(0, 0, 0),
  diffuse_color_(0, 0, 0) {}

Material::Material(const Material &other) {
  ambient_color_ = other.ambient_color_;
  specular_color_ = other.specular_color_;
  diffuse_color_ = other.diffuse_color_;
  shininess_ = other.shininess_;
  if (other.texture_loader_) {
    texture_loader_ = other.texture_loader_;
    transformed_texture_.reset(new Surface(*other.transformed_texture_));
    LoadTexture();
  }
}

Material::Material(Material &&other) = default;

void Material::set_ambient_color(const Vector3 &ambient_color) {
  ambient_color_ = ambient_color;
  //LoadTexture();
}

void Material::set_texture_loader(const std::shared_ptr<SurfaceLoader> &texture_loader) {
  texture_loader_ = texture_loader;
  LoadTexture();
}

void Material::set_shininess(const myfloat shininess) {
  shininess_ = shininess;
}

void Material::set_specular_color(const Vector3 &specular_color) {
  specular_color_ = specular_color;
}

void Material::set_diffuse_color(const Vector3 &diffuse_color) {
  diffuse_color_ = diffuse_color;
}

void Material::LoadTexture() {
  if (!texture_loader_) {
    transformed_texture_.reset();
    return;
  }

  transformed_texture_.reset(new Surface(texture_loader_->LoadSurface()));
#ifdef COMPILE_TIME_BPP
  if (!transformed_texture_->DisplayFormat()) {
    *transformed_texture_ = transformed_texture_->DisplayConvert();
  }
#endif
}
