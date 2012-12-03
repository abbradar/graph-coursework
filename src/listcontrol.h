#ifndef GRAPH_LISTCONTROL_H_
#define GRAPH_LISTCONTROL_H_

#include <vector>
#include <string>
#include "textcontrol.h"
#include "eventworker.h"
#include "cacheddrawer.h"

class ListControl : public TextControl, public CachedDrawer,
  public EventWorker {
 public:
  enum Style {
    kSimple, kEnumerated
  };

  ListControl();

  inline const std::vector<std::string> &items() {
    return items_;
  }

  void set_items(const std::vector<std::string> &items);

  inline Style style() {
    return style_;
  }

  void set_style(const Style style);

  inline int selected() {
    return selected_;
  }

  void set_selected(const int selected);

  unsigned preferred_height();
  unsigned preferred_width();

  virtual bool ProcessMouseMotion(const SDL_MouseMotionEvent &event);
  virtual bool ProcessMouseButtonDown(const SDL_MouseButtonEvent &event);

 protected:
  virtual void Repaint(sdlobj::Surface &surface);

  virtual void OnSelected();

 private:
  Style style_;
  std::vector<std::string> items_;
  int selected_;
  int active_;
};

#endif // GRAPH_LISTCONTROL_H_
