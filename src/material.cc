#include "material.h"

using namespace sdlobj;

Material::Material() = default;


void Material::set_color(const Color &color) {
  color_ = color;
}

void Material::set_texture(const std::shared_ptr<Surface> &texture) {
  texture_ = texture;
}
