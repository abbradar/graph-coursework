#ifndef GRAPH_SCENELOADCONTROL_H_
#define GRAPH_SCENELOADCONTROL_H_

#include "texteditcontrol.h"
#include "context.h"

class SceneLoadControl : public TextEditControl, public ContextUser {
 public:
  SceneLoadControl(const std::shared_ptr<Context> &context);

 protected:
  virtual void OnEntered();
  virtual bool ProcessKeyDown(const SDL_KeyboardEvent &event);
};

#endif // GRAPH_SCENELOADCONTROL_H_
