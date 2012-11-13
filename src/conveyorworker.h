#ifndef GRAPH_CONVEYORWORKER_H_
#define GRAPH_CONVEYORWORKER_H_

class EventWorker;
class PreRenderWorker;
class RenderWorker;
class PostRenderWorker;

class ConveyorWorker {
 public:
  ConveyorWorker();
  virtual ~ConveyorWorker();

  virtual EventWorker *event_worker();
  virtual PreRenderWorker *pre_render_worker();
  virtual RenderWorker *render_worker();
  virtual PostRenderWorker *post_render_worker();
};

class PreRenderWorker : virtual public ConveyorWorker {
 public:
  virtual void PreRenderStep() = 0;

  virtual PreRenderWorker *pre_render_worker();
};

class PostRenderWorker : virtual public ConveyorWorker {
 public:
  virtual void PostRenderStep() = 0;

  virtual PostRenderWorker *post_render_worker();
};


#endif // CONVEYORWORKER_H
