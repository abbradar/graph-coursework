#ifndef GRAPH_RENDERWORKER_H_
#define GRAPH_RENDERWORKER_H_

#include "sdlobj/surface.h"
#include "conveyorworker.h"

class RenderWorker : virtual public ConveyorWorker {
 public:
  RenderWorker();
  virtual ~RenderWorker();

  virtual bool show();
  virtual void set_show(const bool show);

  virtual void Paint(sdlobj::Surface &surface) = 0;

  RenderWorker *render_worker();

 private:
  bool show_;
};

#endif // GRAPH_RENDERWORKER_H_
