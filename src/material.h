#ifndef GRAPH_MATERIAL_H_
#define GRAPH_MATERIAL_H_

#include <memory>
#include "sdlobj/color.h"
#include "sdlobj/surface.h"
#include "myfloat.h"

class Material {
 public:
  Material();
  Material(const Material &other);

  inline const sdlobj::Color &color() const {
    return color_;
  }

  void set_color(const sdlobj::Color &color);

  inline const std::shared_ptr<sdlobj::Surface> &texture() const {
    return texture_;
  }

  void set_texture(const std::shared_ptr<sdlobj::Surface> &texture);

  inline myfloat power() const {
    return power_;
  }

  void set_power(const myfloat power);

  inline const sdlobj::Color &specular_color() const {
    return specular_color_;
  }

  void set_specular_color(const sdlobj::Color &specular_color);

  inline const sdlobj::Color &emissive_color() const {
    return emissive_color_;
  }

  void set_emissive_color(const sdlobj::Color &emissive_color);

 private:
  sdlobj::Color color_;
  myfloat power_;
  sdlobj::Color specular_color_;
  sdlobj::Color emissive_color_;
  std::shared_ptr<sdlobj::Surface> texture_;
};

#endif // GRAPH_MATERIAL_H_
