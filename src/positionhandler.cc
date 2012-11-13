#include "common/math.h"
#include "positionhandler.h"

PositionHandler::PositionHandler(const std::shared_ptr<Context> &context)
  : ContextUser(context), move_speed_(1), rotation_speed_(1) {}

void PositionHandler::set_move_speed(const myfloat move_speed) {
  move_speed_ = move_speed;
  move_step_ = move_speed_ / context()->window->fps();
}

void PositionHandler::set_rotation_speed(const myfloat rotation_speed) {
  rotation_speed_ = rotation_speed;
  rotation_k_ = M_PI_2 / (context()->window->height() * rotation_speed_);
}

bool PositionHandler::ProcessKeyDown(const SDL_KeyboardEvent &event) {
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
    case SDLK_ESCAPE:
      context()->window->set_grab_input(context()->window->grab_input());
      break;
    default:
      return false;
}
return true;
}

bool PositionHandler::ProcessKeyUp(const SDL_KeyboardEvent &event) {
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
    default:
      return false;
  }
  return true;
}

bool PositionHandler::ProcessMouseMotion(const SDL_MouseMotionEvent &event) {
  move_state_.xrel -= event.xrel;
  move_state_.yrel -= event.yrel;
  return true;
}

bool PositionHandler::ProcessResize(const SDL_ResizeEvent &event) {
  rotation_k_ = M_PI_2 / (context()->window->height() * rotation_speed_);
  return false;
}

void PositionHandler::EventStep() {
  context()->window->set_grab_input(true);
  context()->window->set_show_cursor(false);
  move_step_ = move_speed_ / context()->window->fps();
  char forward_back = move_state_.forward + move_state_.back;
  char left_right = move_state_.left + move_state_.right;
  char up_down = move_state_.up + move_state_.down;
  char mul = (abs(forward_back) + abs(left_right) + abs(up_down));
  Position &position = context()->position;
  if (mul > 0) {
    myfloat len = move_step_ / mul;
    if (forward_back) {
      position.z += forward_back * len * sin(position.pitch);
      myfloat prj = forward_back * len * cos(position.pitch);
      position.x += prj * cos(position.yaw);
      position.y += prj * sin(position.yaw);
    }
    if (left_right) {
      myfloat r_yaw = position.yaw + M_PI_2;
      position.x += left_right * len * cos(r_yaw);
      position.y += left_right * len * sin(r_yaw);
    }
    if (up_down) {
      position.z += up_down * len;
    }
  }
  position.pitch += move_state_.yrel * rotation_k_;
  position.pitch = Trim<myfloat>(position.pitch, -M_PI_2, M_PI_2);
  position.yaw += move_state_.xrel * rotation_k_;
  position.yaw = Circle<myfloat>(position.yaw, -M_PI, M_PI);
  move_state_.xrel = 0;
  move_state_.yrel = 0;
}
