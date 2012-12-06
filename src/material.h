#ifndef GRAPH_MATERIAL_H_
#define GRAPH_MATERIAL_H_

#include <memory>
#include "sdlobj/color.h"
#include "sdlobj/surface.h"
#include "vector3.h"
#include "myfloat.h"

class Material {
 public:
  Material();
  Material(const Material &other);

  inline const Vector3 &ambient_color() const {
    return ambient_color_;
  }

  void set_ambient_color(const Vector3 &ambient_color);

  inline const std::shared_ptr<sdlobj::Surface> &texture() const {
    return texture_;
  }

  void set_texture(const std::shared_ptr<sdlobj::Surface> &texture);

  inline myfloat shininess() const {
    return shininess_;
  }

  void set_shininess(const myfloat shininess);

  inline const Vector3 &specular_color() const {
    return specular_color_;
  }

  void set_specular_color(const Vector3 &specular_color);

  inline const Vector3 &emissive_color() const {
    return emissive_color_;
  }

  void set_emissive_color(const Vector3 &emissive_color);

 private:
  myfloat shininess_;
  Vector3 ambient_color_;
  Vector3 specular_color_;
  Vector3 emissive_color_;

  std::shared_ptr<sdlobj::Surface> texture_;
};

#endif // GRAPH_MATERIAL_H_
