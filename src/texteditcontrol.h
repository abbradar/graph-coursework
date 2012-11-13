#ifndef GRAPH_TEXTEDITCONTROL_H_
#define GRAPH_TEXTEDITCONTROL_H_

#include <string>
#include "cachedlabelcontrol.h"

class TextEditControl : public CachedLabelControl {
 public:
  TextEditControl();

  inline unsigned cursor() {
   return cursor_;
  }

  void set_cursor(const unsigned cursor);

  virtual void set_label(const std::string &label);

  virtual void OnEntered();

  virtual void KeyDown(const SDL_keysym &key);

 protected:
  virtual void Repaint(sdlobj::Surface &surface);

 private:
  unsigned cursor_;
};

#endif // GRAPH_TEXTEDITCONTROL_H_
