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
  template <bool kTextures> void TransformObject(const std::shared_ptr<SceneObject> &object, const AffineTransform &transform,
                                                 TransformedObjectMap &new_cache, const Vector3 &normal);

  std::shared_ptr<TransformedObjectMap> cache_;
  ZBuffer z_buffer_;
};

#endif // GRAPH_RASTERIZER_H_
