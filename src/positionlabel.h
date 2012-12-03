#ifndef GRAPH_POSITIONLABEL_H_
#define GRAPH_POSITIONLABEL_H_

#include "fastlabelcontrol.h"
#include "context.h"

class PositionLabel : public FastLabelControl, public ContextUser {
 public:
  PositionLabel(const std::shared_ptr<Context> &context);

  virtual void Paint(sdlobj::Surface &surface);

 private:
};

#endif // GRAPH_POSITIONLABEL_H_
