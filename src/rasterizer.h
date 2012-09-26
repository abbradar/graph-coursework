#ifndef GRAPH_RASTERIZER_H_
#define GRAPH_RASTERIZER_H_

#include <memory>
#include "sdlobj/surface.h"
#include "scene.h"

class Rasterizer {
 public:
  static const float kAOV;

  Rasterizer();

  void Render();

  inline Scene *scene() {
    return scene_;
  }

  void set_scene(Scene *scene);

  inline Position *camera() {
    return camera_;
  }

  void set_camera(Position *camera);

  inline sdlobj::Surface *surface() {
    return surface_;
  }

  void set_surface(sdlobj::Surface *surface);

 private:
  Scene *scene_;
  Position *camera_;
  sdlobj::Surface *surface_;
  std::unique_ptr<float> z_buffer_;
  int z_buffer_size_;
};

#endif // GRAPH_RASTERIZER_H_
