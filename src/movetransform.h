#ifndef GRAPH_MOVETRANSFORM_H_
#define GRAPH_MOVETRANSFORM_H_

#include "matrix4.h"
#include "eventworker.h"
#include "context.h"

class MoveTransform : public EventWorker, public ContextUser, public PostRenderWorker {
 public:
  MoveTransform(const std::shared_ptr<Context> &context,
                const std::shared_ptr<SceneObject> &object);

  bool ProcessMouseMotion(const SDL_MouseMotionEvent &event);
  bool ProcessMouseButtonDown(const SDL_MouseButtonEvent &event);
  bool ProcessKeyDown(const SDL_KeyboardEvent &event);
  bool ProcessKeyUp(const SDL_KeyboardEvent &event);

  void PostRenderStep();

  inline myfloat rotation_speed() {
    return rotation_speed_;
  }

  void set_rotation_speed(const myfloat rotation_speed);

 private:
  static const Point3D kP0, kP1, kP2;
  Point3D p0_, p1_, p2_;

  int xrel_, yrel_;
  std::weak_ptr<SceneObject> object_;
  bool wait_finish_, finish_, rotate_;
  Position old_position_;
  myfloat rotation_speed_;
};

#endif // GRAPH_MOVETRANSFORM_H_