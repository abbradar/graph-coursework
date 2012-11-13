#ifndef GRAPH_FASTLABELCONTROL_H_
#define GRAPH_FASTLABELCONTROL_H_

#include "labelcontrol.h"
#include "renderworker.h"

class FastLabelControl : public LabelControl, public RenderWorker {
 public:
  FastLabelControl();
  ~FastLabelControl();

  inline unsigned x() {
    return x_;
  }

  inline unsigned y() {
    return y_;
  }

  void set_position(unsigned x, unsigned y);

  virtual void Paint(sdlobj::Surface &surface);

 private:
  unsigned x_;
  unsigned y_;
};

#endif // GRAPH_FASTLABELCONTROL_H_
