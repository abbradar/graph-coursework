#include "common/math.h"
#include "movetransform.h"
#include "positionhandler.h"

using namespace std;

PositionHandler::PositionHandler(const std::shared_ptr<Context> &context)
  : ContextUser(context), move_speed_(1), rotation_speed_(1), action_(kNone) {}

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
    case SDLK_t: {
      action_ |= kTransform;
    break;
    }
    case SDLK_g:
      context()->window->set_grab_input(!context()->window->grab_input());
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
  if (context()->window->grab_input()) {
    move_state_.xrel -= event.xrel;
    move_state_.yrel -= event.yrel;
    return true;
  } else {
    return false;
  }
}

bool PositionHandler::ProcessResize(const SDL_ResizeEvent &event) {
  rotation_k_ = M_PI_2 / (context()->window->height() * rotation_speed_);
  context()->camera.set_size(context()->window->width(), context()->window->height());
  return false;
}

void PositionHandler::EventStep() {
  move_step_ = move_speed_ / context()->window->fps();
  char forward_back = move_state_.forward + move_state_.back;
  char left_right = move_state_.left + move_state_.right;
  char up_down = move_state_.up + move_state_.down;
  char mul = (abs(forward_back) + abs(left_right) + abs(up_down));
  Camera &camera = context()->camera;
  if (mul > 0) {
    myfloat len = move_step_ / mul;
    if (forward_back) {
      camera.z += forward_back * len * sin(camera.pitch);
      myfloat prj = forward_back * len * cos(camera.pitch);
      camera.x += prj * cos(camera.yaw);
      camera.y += prj * sin(camera.yaw);
    }
    if (left_right) {
      myfloat r_yaw = camera.yaw + M_PI_2;
      camera.x += left_right * len * cos(r_yaw);
      camera.y += left_right * len * sin(r_yaw);
    }
    if (up_down) {
      camera.z += up_down * len;
    }
  }
  if (context()->window->grab_input()) {
    camera.pitch += move_state_.yrel * rotation_k_;
    camera.pitch = Trim<myfloat>(camera.pitch, -M_PI_2, M_PI_2);
    camera.yaw += move_state_.xrel * rotation_k_;
    camera.yaw = Circle<myfloat>(camera.yaw, -M_PI, M_PI);
  }
  move_state_.xrel = 0;
  move_state_.yrel = 0;
}

void PositionHandler::PreRenderStep() {
  if (action_ & kTransform) {
    std::shared_ptr<SceneObject> object = context()->traced_object.lock()->object.lock();
    if (object) {
      auto ti = make_shared<MoveTransform>(context(), object);
      ti->set_rotation_speed(rotation_speed_);
      context()->window->RegisterWorker(ti);
    }
  }
  action_ = kNone;
}