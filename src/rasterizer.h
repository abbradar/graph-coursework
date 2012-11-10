#ifndef GRAPH_RASTERIZER_H_
#define GRAPH_RASTERIZER_H_

#include <memory>
#include "sdlobj/surface.h"
#include "sdlobj/surfacepainter.h"
#include "screenline3d.h"
#include "scene.h"
#include "myfloat.h"
#include "matrix4.h"
#include "zbuffer.h"

class Rasterizer {
 public:
  struct TraceResult {
    std::weak_ptr<SceneObject> scene_object;
    IndexedTriangle triangle;
  };

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

  void set_viewer_distance(const myfloat viewer_distance);

  inline sdlobj::Surface *surface() {
    return surface_;
  }

  void set_surface(sdlobj::Surface *surface);

  inline myfloat scale() {
    return scale_;
  }

  void set_scale(const myfloat scale);

  void ClearCache();

 private:
  void DrawTriangle(const IndexedTriangle &source, const PointVector &points, const sdlobj::Color &color);
  void FillTriangle(const IndexedTriangle &source, const Point3D *points, const sdlobj::Color &color);
  void FillLines(ScreenLine3D &a, ScreenLine3D &b, const Uint32 color, const unsigned int fy);
  void FillLine(const unsigned int y, const unsigned int ax, const unsigned int bx,
                const myfloat az, const myfloat bz, const Uint32 color);

  inline void SetPixel(const unsigned int x, const unsigned int y, const myfloat z, const Uint32 color) {
    if (z_buffer_.Check(x, y, z)) {
      surface_painter_.SetPixel(x, y, color);
    }
  }

  Scene *scene_;
  Position *camera_;
  sdlobj::Surface *surface_;
  sdlobj::SurfacePainter surface_painter_;
  PointVector point_cache_;
  std::vector<bool> point_flag_cache_;
  TriangleVector triangle_cache_;
  ZBuffer z_buffer_;
  myfloat viewer_distance_;
  myfloat scale_;
};

#endif // GRAPH_RASTERIZER_H_
