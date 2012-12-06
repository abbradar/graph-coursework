#include "common/exception.h"
#include "material.h"

using namespace sdlobj;
using namespace std;

Material::Material() = default;

Material::Material(const Material &other) {
  ambient_color_ = other.ambient_color_;
  specular_color_ = other.specular_color_;
  emissive_color_ = other.emissive_color_;
  shininess_ = other.shininess_;
  if (other.texture_) {
    texture_ = make_shared<Surface>(*other.texture_);
  }
}

void Material::set_ambient_color(const Vector3 &ambient_color) {
  ambient_color_ = ambient_color;
}

void Material::set_texture(const std::shared_ptr<Surface> &texture) {
  texture_ = texture;
}

void Material::set_shininess(const myfloat shininess) {
  shininess_ = shininess;
}

void Material::set_specular_color(const Vector3 &specular_color) {
  specular_color_ = specular_color;
}

void Material::set_emissive_color(const Vector3 &emissive_color) {
  emissive_color_ = emissive_color;
}
