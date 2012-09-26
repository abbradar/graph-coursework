#include "common/logging.h"
#include "common/application.h"
#include "common/math.h"
#include "courseinterface.h"

using namespace sdlobj;

const float CourseInterface::kStep = 2.0f;
const float CourseInterface::kDiagStep = 1.2f;
const float CourseInterface::kRotationStep = 1.0f / 3.0f;

CourseInterface::CourseInterface() {
  rotation_k_ = kPi / 2 / (SDL::instance().surface().height() * kRotationStep);
  set_grab_mouse(false);
}

void CourseInterface::ProcessQuit(const SDL_QuitEvent &) {
  LogDebug("Quit event received; exiting");
  Application::instance().Terminate(0);
}

void CourseInterface::ProcessKeyDown(const SDL_KeyboardEvent &event) {
  switch (event.keysym.sym) {
    case SDLK_w:
      move_state_.up = 1;
      break;
    case SDLK_s:
      move_state_.down = -1;
      break;
    case SDLK_d:
      move_state_.right = 1;
      break;
    case SDLK_a:
      move_state_.left = -1;
      break;
    default:
      break;
  }
}

void CourseInterface::ProcessKeyUp(const SDL_KeyboardEvent &event) {
  switch (event.keysym.sym) {
    case SDLK_w:
      move_state_.up = 0;
      break;
    case SDLK_s:
      move_state_.down = 0;
      break;
    case SDLK_d:
      move_state_.right = 0;
      break;
    case SDLK_a:
      move_state_.left = 0;
      break;
    case SDLK_ESCAPE:
      set_grab_mouse(!grab_mouse());
      break;
    default:
      break;
  }
}

void CourseInterface::ProcessMouseMotion(const SDL_MouseMotionEvent &event) {
  if (!grab_mouse_) return;
  move_state_.xrel += event.xrel;
  move_state_.yrel -= event.yrel;
}

void CourseInterface::ProcessResize(const SDL_ResizeEvent &event) {
  SDL::instance().SetVideoMode(event.w, event.h);
  rotation_k_ = kPi / 2 / (SDL::instance().surface().height() * kRotationStep);
}

void CourseInterface::Step() {
  char up_down = move_state_.up + move_state_.down;
  char left_right = move_state_.left + move_state_.right;
  if (up_down && left_right) {
    position_.x += left_right * kDiagStep;
    position_.y += up_down * kDiagStep;
  } else {
    position_.x += left_right * kStep;
    position_.y += up_down * kStep;
  }
  // TODO: change camera angles
  position_.yaw += move_state_.xrel / rotation_k_;
  position_.pitch += move_state_.yrel / rotation_k_;
  move_state_.xrel = 0;
  move_state_.yrel = 0;
}

void CourseInterface::set_grab_mouse(bool grab) {
  if (grab_mouse_ == grab) return;
  grab_mouse_ = grab;
  if (grab) {
    LogDebug("Started mouse grabbing");
    SDL::instance().set_grab_input(true);
    SDL::instance().set_show_cursor(false);
  } else {
    LogDebug("Stopped mouse grabbing");
    SDL::instance().set_grab_input(false);
    SDL::instance().set_show_cursor(true);
  }
}
