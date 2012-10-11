#ifndef GRAPH_INTERFACE_H_
#define GRAPH_INTERFACE_H_

#include "sdlobj/sdl.h"
#include "scene.h"
#include "position.h"
#include "myfloat.h"

class Interface : public sdlobj::EventHandler {
 public:
  Interface(float fps);

  void ProcessQuit(const SDL_QuitEvent &);
  void ProcessKeyDown(const SDL_KeyboardEvent &);
  void ProcessKeyUp(const SDL_KeyboardEvent &);
  void ProcessMouseMotion(const SDL_MouseMotionEvent &);
  void ProcessResize(const SDL_ResizeEvent &);

  void Step();

  inline Position *position() {
    return position_;
  }

  void set_position(Position *position);

  inline bool grab_mouse() {
    return grab_mouse_;
  }

  void set_grab_mouse(const bool grab);

  /** Speed of movement */
  inline myfloat move_speed() {
    return move_step_;
  }

  void set_move_speed(myfloat move_speed);

  /** Portion of the height of the screen which turns camera 90* */
  inline myfloat rotation_speed() {
    return rotation_speed_;
  }

  void set_rotation_speed(myfloat rotation_speed);

  inline float fps() {
    return fps_;
  }

  inline int x() {
    return x_;
  }

  inline int y() {
    return y_;
  }

  void set_fps(float fps);

 private:
  myfloat move_speed_;
  myfloat rotation_speed_;
  myfloat move_step_;
  myfloat rotation_k_;
  float fps_;

  struct MoveState {
    char forward = 0;
    char back = 0;
    char left = 0;
    char right = 0;
    char up = 0;
    char down = 0;
    int xrel = 0;
    int yrel = 0;
  };

  MoveState move_state_;
  Position *position_;
  int x_, y_;
  bool grab_mouse_;
};

#endif // GRAPH_INTERFACE_H_
