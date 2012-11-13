#ifndef GRAPH_TEXTCONTROL_H_
#define GRAPH_TEXTCONTROL_H_

#include "sdlobj/font.h"
#include "sdlobj/color.h"

class TextControl {
 public:
  TextControl();
  virtual ~TextControl();

  inline const sdlobj::Font &font() {
    return font_;
  }

  virtual void set_font(const sdlobj::Font &font);

  inline const sdlobj::Color font_color() {
    return font_color_;
  }

  virtual void set_font_color(const sdlobj::Color &font_color);

  inline const sdlobj::Color back_color() {
    return back_color_;
  }

  virtual void set_back_color(const sdlobj::Color &back_color);

 private:
  sdlobj::Font font_;
  sdlobj::Color font_color_;
  sdlobj::Color back_color_;
};

#endif // GRAPH_TEXTCONTROL_H_
