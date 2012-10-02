#ifndef GRAPH_RASTERIZER_H_
#define GRAPH_RASTERIZER_H_

#include <memory>
#include "sdlobj/surface.h"
#include "sdlobj/surfacepainter.h"
#include "scene.h"
#include "myfloat.h"
#include "zbuffer.h"

class Rasterizer {
 public:
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

  inline myfloat viewer_distance() {
    return viewer_distance_;
  }

  void set_viewer_distance(myfloat viewer_distance);

  inline sdlobj::Surface *surface() {
    return surface_;
  }

  void set_surface(sdlobj::Surface *surface);

 private:
  void DrawTriangle(const IndexedTriangle &source, const sdlobj::Color &color);
  void FillTriangle(const IndexedTriangle &source, const Point3D *points, const sdlobj::Color &color);

  Scene *scene_;
  Position *camera_;
  sdlobj::Surface *surface_;
  sdlobj::SurfacePainter surface_painter_;
  PointVector point_buffer_;
  ZBuffer z_buffer_;
  myfloat viewer_distance_;
};

#endif // GRAPH_RASTERIZER_H_
