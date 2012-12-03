#ifndef GRAPH_CONTEXTTRACER_H_
#define GRAPH_CONTEXTTRACER_H_

#include "material.h"
#include "context.h"
#include "pointtracer.h"

class ContextTracer : public ContextUser, public PreRenderWorker {
 public:
  ContextTracer(const std::shared_ptr<Context> &context);

  inline Material &material() {
    return *material_;
  }

  void PreRenderStep();

 private:
  std::shared_ptr<TracedObject> traced_object_;
  PointTracer tracer_;
  std::shared_ptr<Material> material_;
};

#endif // GRAPH_CONTEXTTRACER_H_
