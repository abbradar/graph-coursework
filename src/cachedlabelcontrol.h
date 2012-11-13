#ifndef GRAPH_CACHEDLABELCONTROL_H_
#define GRAPH_CACHEDLABELCONTROL_H_

#include "labelcontrol.h"
#include "cacheddrawer.h"

class CachedLabelControl : public CachedDrawer, public LabelControl {
 public:
  CachedLabelControl();

  virtual unsigned preferred_height();
  virtual unsigned preferred_width();

  virtual void set_label(const std::string &label);

  virtual void set_font(const sdlobj::Font &font);
};

#endif // GRAPH_CACHEDLABELCONTROL_H_
