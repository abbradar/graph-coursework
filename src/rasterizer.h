#ifndef GRAPH_RASTERIZER_H_
#define GRAPH_RASTERIZER_H_

#include <memory>
#include <map>
#include "sdlobj/surface.h"
#include "sdlobj/surfacepainter.h"
#include "scene.h"
#include "myfloat.h"
#include "matrix4.h"
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
  template <bool kTextures> void TransformObject(const std::shared_ptr<SceneObject> &object, const Matrix4 &transform,
                                                 TransformedObjectMap &new_cache, const Point3D &normal);

  std::shared_ptr<TransformedObjectMap> cache_;
  ZBuffer z_buffer_;
};

#endif // GRAPH_RASTERIZER_H_
