#ifndef GRAPH_MATERIAL_H_
#define GRAPH_MATERIAL_H_

#include <memory>
#include "sdlobj/color.h"
#include "sdlobj/surface.h"

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

 private:
  sdlobj::Color color_;
  std::shared_ptr<sdlobj::Surface> texture_;
};

#endif // GRAPH_MATERIAL_H_
