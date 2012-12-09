#ifndef GRAPH_OBJECTMENU_H_
#define GRAPH_OBJECTMENU_H_

#include "listcontrol.h"
#include "context.h"

class ObjectMenu : public ListControl, public ContextUser {
 public:
  ObjectMenu(const std::shared_ptr<Context> &context);

 protected:
  virtual void OnSelected();
  virtual bool ProcessKeyDown(const SDL_KeyboardEvent &event);
};

#endif // GRAPH_OBJECTMENU_H_
