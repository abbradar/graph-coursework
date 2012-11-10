#include <cmath>
#include "common/logging.h"
#include "common/application.h"
#include "common/math.h"
#include "common/exception.h"
#include "interface.h"

//#define NO_MOUSE_GRAB

using namespace sdlobj;
using namespace std;

Interface::Interface(float fps) : move_speed_(1), rotation_speed_(1), fps_(fps),
  position_(nullptr), x_(0), y_(0), grab_mouse_(false) {
  set_grab_mouse(false);
  set_fps(fps_);
}

Interface::Interface() : Interface(1) {}

void Interface::set_move_speed(myfloat move_speed) {
  move_speed_ = move_speed;
  move_step_ = move_speed_ / fps_;
}

void Interface::set_rotation_speed(myfloat rotation_speed) {
  rotation_speed_ = rotation_speed;
  rotation_k_ = M_PI_2 / (SDL::instance().surface().height() * rotation_speed_);
}

void Interface::set_fps(float fps) {
  if (fps <= 0) {
    throw Exception("FPS should be > 0");
  }
  fps_ = fps;
  set_move_speed(move_speed_);
}

void Interface::ProcessQuit(const SDL_QuitEvent &) {
  LogDebug("Quit event received; exiting");
  Application::instance().Terminate(EXIT_SUCCESS);
}

void Interface::ProcessKeyDown(const SDL_KeyboardEvent &event) {
  switch (event.keysym.sym) {
    case SDLK_w:
      move_state_.forward = 1;
      break;
    case SDLK_s:
      move_state_.back = -1;
      break;
    case SDLK_d:
      move_state_.right = -1;
      break;
    case SDLK_a:
      move_state_.left = 1;
      break;
    case SDLK_SPACE:
      move_state_.up = 1;
      break;
    case SDLK_LCTRL:
      move_state_.down = -1;
      break;
    default:
      break;
  }
}

void Interface::ProcessKeyUp(const SDL_KeyboardEvent &event) {
  switch (event.keysym.sym) {
    case SDLK_w:
      move_state_.forward = 0;
      break;
    case SDLK_s:
      move_state_.back = 0;
      break;
    case SDLK_d:
      move_state_.right = 0;
      break;
    case SDLK_a:
      move_state_.left = 0;
      break;
    case SDLK_SPACE:
      move_state_.up = 0;
      break;
    case SDLK_LCTRL:
      move_state_.down = 0;
      break;
    case SDLK_ESCAPE:
      set_grab_mouse(!grab_mouse());
      break;
    default:
      break;
  }
}

void Interface::ProcessMouseMotion(const SDL_MouseMotionEvent &event) {
  x_ = event.x;
  y_ = event.y;
  if (grab_mouse_) {
    move_state_.xrel -= event.xrel;
    move_state_.yrel -= event.yrel;
  }
}

void Interface::ProcessResize(const SDL_ResizeEvent &event) {
  SDL::instance().SetVideoMode(event.w, event.h);
  set_rotation_speed(rotation_speed_);
}

void Interface::Step() {
  if (!position_) throw Exception("Position pointer is not setted.");
  char forward_back = move_state_.forward + move_state_.back;
  char left_right = move_state_.left + move_state_.right;
  char up_down = move_state_.up + move_state_.down;
  char mul = (abs(forward_back) + abs(left_right) + abs(up_down));
  if (mul > 0) {
    myfloat len = move_step_ / mul;
    if (forward_back) {
      position_->z += forward_back * len * sin(position_->pitch);
      myfloat prj = forward_back * len * cos(position_->pitch);
      position_->x += prj * cos(position_->yaw);
      position_->y += prj * sin(position_->yaw);
    }
    if (left_right) {
      myfloat r_yaw = position_->yaw + M_PI_2;
      position_->x += left_right * len * cos(r_yaw);
      position_->y += left_right * len * sin(r_yaw);
    }
    if (up_down) {
      position_->z += up_down * len;
    }
  }
  position_->pitch += move_state_.yrel * rotation_k_;
  position_->pitch = Trim<myfloat>(position_->pitch, -M_PI_2, M_PI_2);
  position_->yaw += move_state_.xrel * rotation_k_;
  position_->yaw = Circle<myfloat>(position_->yaw, -M_PI, M_PI);
  move_state_.xrel = 0;
  move_state_.yrel = 0;
}

void Interface::set_position(Position *position) {
  position_ = position;
}

void Interface::set_grab_mouse(const bool grab) {
  if (grab_mouse_ == grab) return;
  grab_mouse_ = grab;
  if (grab_mouse_) {
    LogDebug("Started mouse grabbing");
  } else {
    LogDebug("Stopped mouse grabbing");
  }
#ifndef NO_MOUSE_GRAB
  SDL::instance().set_grab_input(grab_mouse_);
#endif
  SDL::instance().set_show_cursor(!grab_mouse_);
}
