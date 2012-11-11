#ifndef GRAPH_RASTERIZER_H_
#define GRAPH_RASTERIZER_H_

#include <memory>
#include <map>
#include "sdlobj/surface.h"
#include "sdlobj/surfacepainter.h"
#include "screenline3d.h"
#include "pointtracer.h"
#include "scene.h"
#include "myfloat.h"
#include "matrix4.h"
#include "zbuffer.h"

class Rasterizer {
 public:
  Rasterizer();

  void FirstPass();
  void SecondPass();

  inline const Scene *scene() {
    return scene_;
  }

  void set_scene(const Scene *scene);

  inline const Position *camera() {
    return camera_;
  }

  void set_camera(const Position *camera);

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

  inline bool trace() {
    return trace_;
  }

  void set_trace(const bool trace);

  inline myfloat view_limit() {
    return view_limit_;
  }

  void set_view_limit(const myfloat view_limit);

  inline unsigned trace_x() {
    return point_tracer_.x();
  }

  inline unsigned trace_y() {
    return point_tracer_.y();
  }

  void set_trace_point(const unsigned x, const unsigned y);

  inline std::weak_ptr<SceneObject> traced_object() {
    return traced_object_;
  }

  void ClearCache();

 private:
  struct ObjectCache {
    ObjectCache(const std::weak_ptr<SceneObject> &object);

    std::weak_ptr<SceneObject> object;
    Point3DVector points;
    std::vector<bool> point_flags;
    TriangleVector triangles;
    std::vector<size_t> material_indexes;
  };

  typedef std::map<SceneObject *, std::unique_ptr<ObjectCache>> CacheMap;
  typedef std::pair<SceneObject *, std::unique_ptr<ObjectCache>> CacheMapPair;

  void DrawTriangle(const IndexedTriangle &source, const Point3DVector &points, const Model &model,
                    const Material &material);
  void FillTriangle(const IndexedTriangle &source, const Point3D *points, const Material &material);
  void FillLines(ScreenLine3D &a, ScreenLine3D &b, const Uint32 color, const unsigned int fy);
  void FillLine(const unsigned int y, const unsigned int ax, const unsigned int bx,
                const myfloat az, const myfloat bz, const Uint32 color);

  inline void SetPixel(const unsigned int x, const unsigned int y, const myfloat z, const Uint32 color) {
    if (z_buffer_.Check(x, y, z)) {
      surface_painter_.SetPixel(x, y, color);
    }
  }

  const Scene *scene_;
  const Position *camera_;
  sdlobj::Surface *surface_;
  sdlobj::SurfacePainter surface_painter_;
  CacheMap cache_;
  bool trace_;
  PointTracer point_tracer_;
  std::weak_ptr<SceneObject> traced_object_;
  ZBuffer z_buffer_;
  myfloat viewer_distance_;
  myfloat scale_;
  myfloat view_limit_;
};

#endif // GRAPH_RASTERIZER_H_
