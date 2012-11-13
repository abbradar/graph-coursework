#ifndef GRAPH_POSITIONLABEL_H_
#define GRAPH_POSITIONLABEL_H_

#include "textcontrol.h"
#include "renderworker.h"
#include "context.h"

class PositionLabel : public TextControl, public RenderWorker, public ContextUser {
 public:
  PositionLabel(const std::shared_ptr<Context> &context);

  inline unsigned x() {
    return x_;
  }

  inline unsigned y() {
    return y_;
  }

  void set_position(const unsigned x, const unsigned y);

  virtual void Paint(sdlobj::Surface &surface);

 private:
  unsigned x_;
  unsigned y_;
};

#endif // GRAPH_POSITIONLABEL_H_
