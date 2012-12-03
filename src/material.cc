#include "common/exception.h"
#include "material.h"

using namespace sdlobj;
using namespace std;

Material::Material() = default;

Material::Material(const Material &other) {
  color_ = other.color_;
  if (other.texture_) {
    texture_ = make_shared<Surface>(*other.texture_);
  }
}

void Material::set_color(const Color &color) {
  color_ = color;
}

void Material::set_texture(const std::shared_ptr<Surface> &texture) {
  texture_ = texture;
}

void Material::set_power(const myfloat power) {
  //if (power_ <= 0 || power_ >= 1) throw Exception("Power should be between 0 and 1.");
  power_ = power;  
}

void Material::set_specular_color(const sdlobj::Color &specular_color) {
  specular_color_ = specular_color;
}

void Material::set_emissive_color(const sdlobj::Color &emissive_color) {
  emissive_color_ = emissive_color;
}
