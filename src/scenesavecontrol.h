#ifndef GRAPH_SCENESAVECONTROL_H_
#define GRAPH_SCENESAVECONTROL_H_

#include "texteditcontrol.h"
#include "context.h"

class SceneSaveControl : public TextEditControl, public ContextUser {
 public:
  SceneSaveControl(const std::shared_ptr<Context> &context);

 protected:
  virtual void OnEntered();
  virtual bool ProcessKeyDown(const SDL_KeyboardEvent &event);
};

#endif // GRAPH_SCENESAVECONTROL_H_
