#ifndef GRAPH_RASTERIZER_H_
#define GRAPH_RASTERIZER_H_

#include <memory>
#include "sdlobj/surface.h"
#include "sdlobj/surfacepainter.h"
#include "scene.h"
#include "myfloat.h"

class Rasterizer {
 public:
  static const myfloat kFOVX;
  static const myfloat kFOVY;

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
  void DrawTriangle(const IndexedPolygon &index, const SceneObject::PointVector &points, const sdlobj::Color &color);
  void ResetZBuffer();

  Scene *scene_;
  Position *camera_;
  sdlobj::Surface *surface_;
  sdlobj::SurfacePainter surface_painter_;
  std::unique_ptr<myfloat> z_buffer_;
  int z_buffer_size_;
};

#endif // GRAPH_RASTERIZER_H_
