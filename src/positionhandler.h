#ifndef GRAPH_POSITIONHANDLER_H_
#define GRAPH_POSITIONHANDLER_H_

#include "context.h"
#include "eventworker.h"

class PositionHandler : public EventWorker, public PreRenderWorker, public ContextUser {
 public:
  PositionHandler(const std::shared_ptr<Context> &context);

  virtual bool ProcessKeyDown(const SDL_KeyboardEvent &event);
  virtual bool ProcessKeyUp(const SDL_KeyboardEvent &event);
  virtual bool ProcessMouseMotion(const SDL_MouseMotionEvent &event);
  virtual bool ProcessResize(const SDL_ResizeEvent &event);

  virtual void EventStep();
  virtual void PreRenderStep();

  /** Speed of movement */
  inline myfloat move_speed() {
    return move_step_;
  }

  void set_move_speed(const myfloat move_speed);

  /** Portion of the height of the screen which turns camera 90* */
  inline myfloat rotation_speed() {
    return rotation_speed_;
  }

  void set_rotation_speed(const myfloat rotation_speed);

  inline myfloat keys_rotation_step() {
    return keys_rotation_step_;
  }

  void set_keys_rotation_step(const myfloat keys_rotation_step);

 private:
  enum PreRenderAction {
    kNone = 0,
    kGrabInput = 0b1,
    kTransform = 0b10,
    kWeld = 0b100,
    kAdd = 0b1000,
    kRemove = 0b10000,
    kLoad = 0b100000,
    kSave = 0b1000000
  };

  struct MoveState {
    char forward = 0;
    char back = 0;
    char left = 0;
    char right = 0;
    char up = 0;
    char down = 0;
    char cam_up = 0;
    char cam_down = 0;
    char cam_left = 0;
    char cam_right = 0;
    int xrel = 0;
    int yrel = 0;
  };

  myfloat move_speed_;
  myfloat rotation_speed_;
  myfloat keys_rotation_step_;
  myfloat move_step_;
  myfloat rotation_k_;

  int action_;

  MoveState move_state_;
};

#endif // POSITIONHANDLER_H
