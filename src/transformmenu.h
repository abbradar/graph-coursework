#ifndef TRANSFORMMENU_H
#define TRANSFORMMENU_H

#include "listcontrol.h"
#include "eventworker.h"
#include "context.h"

class TransformMenu : public ListControl, public PreRenderWorker,
  public ContextUser {
 public:
  TransformMenu(const std::shared_ptr<Context> &context);

  void PreRenderStep();

  virtual bool ProcessKeyDown(const SDL_KeyboardEvent &event);

 protected:
  void OnSelected();

 private:
  enum Action {
    kNone, kMove, kRotate, kAttach
  };

  Action action_;
};

#endif // TRANSFORMMENU_H
