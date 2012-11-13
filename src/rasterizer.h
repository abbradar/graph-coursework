#ifndef GRAPH_RASTERIZER_H_
#define GRAPH_RASTERIZER_H_

#include <memory>
#include <map>
#include "sdlobj/surface.h"
#include "sdlobj/surfacepainter.h"
#include "screenline3d.h"
#include "scene.h"
#include "myfloat.h"
#include "matrix4.h"
#include "zbuffer.h"
#include "renderworker.h"
#include "context.h"

class Rasterizer : public RenderWorker, public ContextUser {
 public:
  Rasterizer(const std::shared_ptr<Context> &context);

  void Prepare();
  void Clear();

  virtual void Paint(sdlobj::Surface &surface);

  inline myfloat viewer_distance() {
    return viewer_distance_;
  }

  void set_viewer_distance(const myfloat viewer_distance);

  inline myfloat scale() {
    return scale_;
  }

  void set_scale(const myfloat scale);

  inline myfloat view_limit() {
    return view_limit_;
  }

  void set_view_limit(const myfloat view_limit);

 private:
  void DrawTriangle(const IndexedTriangle &source, const Point3DVector &points,
                    const Model &model, const Material &material);
  void FillTriangle(const IndexedTriangle &source, const Point3D *points, const Material &material);
  void FillLines(ScreenLine3D &a, ScreenLine3D &b, const Uint32 color, const unsigned int fy);
  void FillLine(const ScreenLine3D &a, const ScreenLine3D &b, const Uint32 color);
  inline void SetPixel(const unsigned int x, const unsigned int y, const myfloat z, const Uint32 color);

  sdlobj::SurfacePainter surface_painter_;
  std::shared_ptr<TransformedObjectMap> cache_;
  ZBuffer z_buffer_;
  myfloat viewer_distance_;
  myfloat scale_;
  myfloat view_limit_;
  unsigned width_, height_;
};

#endif // GRAPH_RASTERIZER_H_
