#ifndef GRAPH_BLENDEDLABELCONTROL_H_
#define GRAPH_BLENDEDLABELCONTROL_H_

#include "cachedlabelcontrol.h"

class BlendedLabelControl : public CachedLabelControl {
 public:
  BlendedLabelControl();

 protected:
  virtual void Repaint(sdlobj::Surface &surface);
};

#endif // GRAPH_BLENDEDLABELCONTROL_H_
