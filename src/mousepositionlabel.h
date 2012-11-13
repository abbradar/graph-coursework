#ifndef GRAPH_MOUSEPOSITIONLABEL_H_
#define GRAPH_MOUSEPOSITIONLABEL_H_

#include "fastlabelcontrol.h"
#include "eventworker.h"
#include "context.h"

class MousePositionLabel : public FastLabelControl, public EventWorker,
    public ContextUser {
 public:
  MousePositionLabel(const std::shared_ptr<Context> &context);

  virtual void EventStep();

 private:
  unsigned x_;
  unsigned y_;
};

#endif // GRAPH_MOUSEPOSITIONLABEL_H_
