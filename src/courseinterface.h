#ifndef GRAPH_COURSEINTERFACE_H_
#define GRAPH_COURSEINTERFACE_H_

#include "sdlobj/sdl.h"

class CourseInterface : public sdlobj::EventHandler {
 public:
  struct Position {
    float x = 0;
    float y = 0;
    float pitch = 0;
    float yaw = 0;
  };

  CourseInterface();

  void ProcessQuit(const SDL_QuitEvent &);
  void ProcessKeyDown(const SDL_KeyboardEvent &);
  void ProcessKeyUp(const SDL_KeyboardEvent &);
  void ProcessMouseMotion(const SDL_MouseMotionEvent &);
  void ProcessResize(const SDL_ResizeEvent &);

  void Step();

  inline const Position &position() const {
    return position_;
  }

 private:
  static constexpr float kStep = 2.0f;
  static constexpr float kDiagStep = 1.2f;
  /** Portion of the height of the screen which turns camera 90* */
  static constexpr float kRotationStep = 1.0f / 3.0f;

  float rotation_k_;

  struct MoveState {
    char up = 0;
    char down = 0;
    char left = 0;
    char right = 0;
    int xrel = 0;
    int yrel = 0;
  };

  MoveState move_state_;
  Position position_;
};

#endif // GRAPH_COURSEINTERFACE_H_
