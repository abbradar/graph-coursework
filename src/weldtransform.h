#ifndef GRAPH_WELDTRANSFORM_H_
#define GRAPH_WELDTRANSFORM_H_

#include "vector3.h"
#include "eventworker.h"
#include "context.h"

class WeldTransform : public EventWorker, public ContextUser, public PostRenderWorker {
 public:
  WeldTransform(const std::shared_ptr<Context> &context);

  struct MoveState {
    char cam_up = 0;
    char cam_down = 0;
    char cam_left = 0;
    char cam_right = 0;
  };

  bool ProcessMouseMotion(const SDL_MouseMotionEvent &event);
  bool ProcessMouseButtonDown(const SDL_MouseButtonEvent &event);
  bool ProcessKeyDown(const SDL_KeyboardEvent &event);
  bool ProcessKeyUp(const SDL_KeyboardEvent &event);

  void PostRenderStep();

  void ProcessEnter();

  inline myfloat rotation_speed() {
    return rotation_speed_;
  }

  void set_rotation_speed(const myfloat rotation_speed);

 private:
  Vector3 tp_, tn_, tnl_;

  MoveState move_state_;
  int xrel_, yrel_;
  std::weak_ptr<SceneObject> object_;
  bool wait_finish_, finish_, rotate_;
  Position old_position_;
  myfloat rotation_speed_;
};

#endif // GRAPH_WELDTRANSFORM_H_
