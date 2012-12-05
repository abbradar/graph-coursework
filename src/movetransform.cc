#include "common/exception.h"
#include "common/math.h"
#include "point2d.h"
#include "movetransform.h"

const Vector3 MoveTransform::kP0 = Vector3(0, 0, 0);
const Vector3 MoveTransform::kP1 = Vector3::UnitX();
const Vector3 MoveTransform::kP2 = Vector3::UnitY();
const Vector3 MoveTransform::kP3 = Vector3::UnitZ();

MoveTransform::MoveTransform(const std::shared_ptr<Context> &context) :
 ContextUser(context), xrel_(0), yrel_(0), wait_finish_(false), finish_(false),
  rotate_(false), z_rotate_(false), rotation_speed_(1) {
  context->window->set_grab_input(true);
  auto traced = context->traced_object.lock();
  object_ = traced->object;
  auto lock = object_.lock();
  if (!lock) {
    throw Exception("Move transform should be created with traced object in context.");
  }

  old_position_ = lock->position();
  tp_.x() = traced->x;
  tp_.y() = traced->y;
  tp_.z() = traced->z;
  context->camera.ReversePerspectiveTransform(tp_);
  tp_ = Vector3(tp_.z(), -tp_.x(), -tp_.y());
  tp_ = old_position_.GetMatrixTo() * context->camera.GetMatrixFrom() * tp_;

  AffineTransform obj_transform = context->camera.GetMatrixTo() * old_position_.GetMatrixFrom();
  p0_ = obj_transform * kP0;
  p1_ = obj_transform * kP1;
  p2_ = obj_transform * kP2;
}

bool MoveTransform::ProcessMouseMotion(const SDL_MouseMotionEvent &event) {
  if (!(rotate_ || z_rotate_)) return false;
  xrel_ -= event.xrel;
  yrel_ -= event.yrel;
  return true;
}

bool MoveTransform::ProcessMouseButtonDown(const SDL_MouseButtonEvent &event) {
  switch (event.button) {
    case SDL_BUTTON_LEFT:
      wait_finish_ = true;
      break;
    default:
      break;
  }
  return false;
}

bool MoveTransform::ProcessKeyDown(const SDL_KeyboardEvent &event) {
  switch (event.keysym.sym) {
    case SDLK_ESCAPE: {
      auto lock = object_.lock();
      if (lock) {
        lock->set_position(old_position_);
      }
      context()->window->UnregisterWorker(this);
      finish_ = true;
      break;
    } case SDLK_RETURN:
      wait_finish_ = true;
      break;
    case SDLK_r:
      rotate_ = true;
      return true;
    case SDLK_z:
      z_rotate_ = true;
      return true;
    case SDLK_g:
      return true;
    case SDLK_KP8:
      move_state_.cam_up = 1;
      if (rotate_ || z_rotate_)
        return true;
      break;
    case SDLK_KP2:
      move_state_.cam_down = -1;
      if (rotate_ || z_rotate_)
        return true;
      break;
    case SDLK_KP4:
      move_state_.cam_left = 1;
      if (rotate_ || z_rotate_)
        return true;
      break;
    case SDLK_KP6:
      move_state_.cam_right = -1;
      if (rotate_ || z_rotate_)
        return true;
      break;
    default:
      break;
  }
  return false;
}

bool MoveTransform::ProcessKeyUp(const SDL_KeyboardEvent &event) {
  switch (event.keysym.sym) {
    case SDLK_r:
      rotate_ = false;
      return true;
    case SDLK_z:
      z_rotate_ = false;
      return true;
    case SDLK_KP8:
      move_state_.cam_up = 0;
      break;
    case SDLK_KP2:
      move_state_.cam_down = 0;
      break;
    case SDLK_KP4:
      move_state_.cam_left = 0;
      break;
    case SDLK_KP6:
      move_state_.cam_right = 0;
      break;
    default:
      break;
  }
  return false;
}

void MoveTransform::PostRenderStep() {
  if (finish_) return;

  auto lock = object_.lock();
  if (!lock) {
    context()->window->UnregisterWorker(this);
    return;
  }

  if (z_rotate_ || rotate_) {
    myfloat rotation_k_ = M_PI_2 / (context()->window->height() * rotation_speed_);

    Vector3 axis(0, 0, 0);
    char cam_left_right = move_state_.cam_left + move_state_.cam_right;
    char cam_up_down = move_state_.cam_up + move_state_.cam_down;
    xrel_ += cam_left_right * 10;
    yrel_ -= cam_up_down * 10;
    myfloat dist = sqrt(Sqr(xrel_) + Sqr(yrel_));
    if (dist != 0) {
      if (z_rotate_) {
        axis.x() = 1;
      } else {
        axis.z() = -xrel_ / dist;
        axis.y() = -yrel_ / dist;
      }
      myfloat angle = dist * rotation_k_;

      const Position &old_position = lock->position();

      Vector3 u = context()->camera.GetRotateMatrixFrom() * axis;
      Vector3 ttp = old_position.GetMatrixFrom() * tp_;

      AffineTransform matrix = TranslateTransform(ttp.x(), ttp.y(), ttp.z())
          * RotateTransform(angle, u) * TranslateTransform(-ttp.x(), -ttp.y(), -ttp.z())
          * old_position.GetMatrixFrom();

      Position new_position;

      TransformToTranslate(matrix, new_position.x, new_position.y, new_position.z);
      TransformToEuler(matrix, new_position.roll, new_position.pitch, new_position.yaw);

      lock->set_position(new_position);
    }
  } else {
    AffineTransform from_camera = context()->camera.GetMatrixFrom();
    Vector3 n_p0 = from_camera * p0_,
            n_p1 = from_camera * p1_,
            n_p2 = from_camera * p2_;
    Position new_position;
    new_position.x = n_p0.x();
    new_position.y = n_p0.y();
    new_position.z = n_p0.z();
    n_p1 -= n_p0;
    n_p2 -= n_p0;
    new_position.yaw = Angle(Vector2(n_p1.x(), n_p1.y()));
    AffineTransform z_transform(RotateTransform(-new_position.yaw, Vector3::UnitZ()));
    n_p1 = z_transform * n_p1;
    n_p2 = z_transform * n_p2;
    new_position.pitch = -Angle(Vector2(n_p1.x(), n_p1.z()));
    AffineTransform y_transform(RotateTransform(-new_position.pitch, Vector3::UnitY()));
    n_p2 = y_transform * n_p2;
    new_position.roll = Angle(Vector2(n_p2.y(), n_p2.z()));

    lock->set_position(new_position);
  }

  if (wait_finish_) {
    context()->window->UnregisterWorker(this);
    finish_ = true;
  }

  xrel_ = 0;
  yrel_ = 0;
}

void MoveTransform::set_rotation_speed(const myfloat rotation_speed) {
  rotation_speed_ = rotation_speed;
}
