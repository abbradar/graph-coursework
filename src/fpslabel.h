#ifndef GRAPH_FPSLABEL_H_
#define GRAPH_FPSLABEL_H_

#include "cachedlabelcontrol.h"
#include "eventworker.h"
#include "context.h"

class FPSLabel : public CachedLabelControl,
    public EventWorker, public ContextUser {
 public:
  FPSLabel(const std::shared_ptr<Context> &context);

  unsigned fps_step();
  void set_fps_step(const unsigned fps_step);

  virtual void EventStep();

 protected:
  virtual void Repaint(sdlobj::Surface &surface);

 private:
  unsigned fps_step_;
  unsigned curr_step_;
};

#endif // GRAPH_FPSLABEL_H_
