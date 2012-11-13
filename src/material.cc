#include "material.h"

using namespace sdlobj;
using namespace std;

Material::Material() = default;

void Material::set_color(const Color &color) {
  color_ = color;
}

void Material::set_texture(const std::shared_ptr<Surface> &texture) {
  texture_ = texture;
}

Material::Material(const Material &other) {
  color_ = other.color_;
  if (other.texture_) {
    texture_ = make_shared<Surface>(*other.texture_);
  }
}
