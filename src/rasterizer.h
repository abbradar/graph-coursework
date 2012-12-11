#ifndef GRAPH_RASTERIZER_H_
#define GRAPH_RASTERIZER_H_

#include <memory>
#include "sdlobj/surface.h"
#include "sdlobj/color.h"
#include "myfloat.h"
#include "zbuffer.h"
#include "renderworker.h"
#include "context.h"

class Rasterizer : public PreRenderWorker, RenderWorker, public ContextUser {
 public:
  Rasterizer(const std::shared_ptr<Context> &context);

  void PreRenderStep();
  void Clear();

  virtual void Paint(sdlobj::Surface &surface);

  inline const sdlobj::Color &surface_color() const {
    return surface_color_;
  }

  void set_surface_color(const sdlobj::Color &surface_color);

 private:
  std::shared_ptr<TransformedObjectMap> cache_;
  ZBuffer z_buffer_;
  sdlobj::Color surface_color_;
};

#endif // GRAPH_RASTERIZER_H_
