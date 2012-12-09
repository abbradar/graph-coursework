#ifndef GRAPH_RASTERIZER_H_
#define GRAPH_RASTERIZER_H_

#include <memory>
#include "sdlobj/surface.h"
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

 private:
  std::shared_ptr<TransformedObjectMap> cache_;
  ZBuffer z_buffer_;
};

#endif // GRAPH_RASTERIZER_H_
