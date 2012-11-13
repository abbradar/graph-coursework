#ifndef GRAPH_CACHEDTEXTCONTROL_H_
#define GRAPH_CACHEDTEXTCONTROL_H_

#include "textcontrol.h"
#include "cacheddrawer.h"

class CachedTextControl : virtual public TextControl, public CachedDrawer {
 public:
  CachedTextControl();

  virtual void set_font(const sdlobj::Font &font);

  virtual void set_font_color(const sdlobj::Color &font_color);

  virtual void set_back_color(const sdlobj::Color &back_color);
};

#endif // GRAPH_CACHEDTEXTCONTROL_H_
