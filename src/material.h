#ifndef GRAPH_MATERIAL_H_
#define GRAPH_MATERIAL_H_

#include <memory>
#include "sdlobj/color.h"
#include "sdlobj/surface.h"
#include "vector3.h"
#include "myfloat.h"
#include "surfaceloader.h"

class Material {
 public:
  Material();
  Material(const Material &other);
  Material(Material &&other);

  inline const Vector3 &ambient_color() const {
    return ambient_color_;
  }

  void set_ambient_color(const Vector3 &ambient_color);

  inline sdlobj::Color color() const {
    return color_;
  }

  inline const std::shared_ptr<SurfaceLoader> &texture_loader() const {
    return texture_loader_;
  }

  void set_texture_loader(const std::shared_ptr<SurfaceLoader> &texture_loader);

  inline const std::unique_ptr<sdlobj::Surface> &texture() const {
    return transformed_texture_;
  }

  inline myfloat shininess() const {
    return shininess_;
  }

  void set_shininess(const myfloat shininess);

  inline const Vector3 &specular_color() const {
    return specular_color_;
  }

  void set_specular_color(const Vector3 &specular_color);

  inline const Vector3 &diffuse_color() const {
    return diffuse_color_;
  }

  void set_diffuse_color(const Vector3 &diffuse_color);

  inline bool lighting() const {
    return lighting_;
  }

  void set_lighting(const bool lighting);

 private:
  void LoadTexture();

  myfloat shininess_;
  Vector3 ambient_color_;
  Vector3 specular_color_;
  Vector3 diffuse_color_;

  sdlobj::Color color_;

  std::shared_ptr<SurfaceLoader> texture_loader_;
  std::unique_ptr<sdlobj::Surface> transformed_texture_;

  bool lighting_;
};

#endif // GRAPH_MATERIAL_H_
