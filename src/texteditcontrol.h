#ifndef GRAPH_TEXTEDITCONTROL_H_
#define GRAPH_TEXTEDITCONTROL_H_

#include <string>
#include "cachedlabelcontrol.h"
#include "eventworker.h"

class TextEditControl : public CachedLabelControl, public EventWorker {
 public:
  TextEditControl();
  ~TextEditControl();

  inline unsigned cursor() {
   return cursor_;
  }

  void set_cursor(const unsigned cursor);

  inline const std::string &caption() {
    return caption_;
  }

  void set_caption(const std::string &caption);

  virtual unsigned preferred_width();

  virtual void set_label(const std::string &label);

  virtual bool ProcessKeyDown(const SDL_KeyboardEvent &event);

 protected:
  virtual void OnEntered();

  virtual void Repaint(sdlobj::Surface &surface);

 private:
  bool translate_keysym_;
  unsigned cursor_;
  std::string caption_;
};

#endif // GRAPH_TEXTEDITCONTROL_H_
