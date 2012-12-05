#include "common/exception.h"
#include "common/math.h"
#include "point2d.h"
#include "weldtransform.h"

WeldTransform::WeldTransform(const std::shared_ptr<Context> &context) :
 ContextUser(context), xrel_(0), yrel_(0), wait_finish_(false), finish_(false),
  rotate_(false), rotation_speed_(1) {
  auto traced = context->traced_object.lock();
  object_ = traced->object;
  auto lock = object_.lock();
  if (!lock) {
    throw Exception("Weld transform should be created with traced object in context.");
  }

  old_position_ = lock->position();
  tp_.x() = traced->x;
  tp_.y() = traced->y;
  tp_.z() = traced->z;
  context->camera.ReversePerspectiveTransform(tp_);
  tp_ = Vector3(tp_.z(), -tp_.x(), -tp_.y());
  tp_ = old_position_.GetMatrixTo() * context->camera.GetMatrixFrom() * tp_;

  tn_ = lock->positioned_polygon_normals()[traced->triangle_index];
}

bool WeldTransform::ProcessMouseMotion(const SDL_MouseMotionEvent &event) {
  if (!rotate_) return false;
  xrel_ -= event.xrel;
  yrel_ -= event.yrel;
  return true;
}

bool WeldTransform::ProcessMouseButtonDown(const SDL_MouseButtonEvent &event) {
  switch (event.button) {
    case SDL_BUTTON_LEFT:
      ProcessEnter();
      break;
    default:
      break;
  }
  return false;
}

void WeldTransform::ProcessEnter() {
  if (rotate_) {
    wait_finish_ = true;
  } else {
    auto traced = context()->traced_object.lock();
    auto new_lock = traced->object.lock();
    auto lock = object_.lock();
    if (!new_lock || new_lock == lock) return;
    if (!lock) {
      context()->window->UnregisterWorker(this);
      finish_ = true;
      return;
    }

    Vector3 np;
    np.x() = traced->x;
    np.y() = traced->y;
    np.z() = traced->z;
    context()->camera.ReversePerspectiveTransform(np);
    np = Vector3(np.z(), -np.x(), -np.y());
    np = context()->camera.GetMatrixFrom() * np;

    Vector3 nn = new_lock->positioned_polygon_normals()[traced->triangle_index] * -1;

    Position new_position;
    new_position.yaw = Angle(Vector2(tn_.x(), tn_.y()), Vector2(nn.x(), nn.y()));
    AffineTransform z_transform(RotateTransform(-new_position.yaw, Vector3::UnitZ()));
    nn = z_transform * nn;
    tn_ = z_transform * tn_;
    new_position.pitch = Angle(Vector2(tn_.x(), tn_.z()), Vector2(nn.x(), nn.z()));
    AffineTransform y_transform(RotateTransform(-new_position.pitch, Vector3::UnitY()));
    nn = y_transform * nn;
    tn_ = y_transform * tn_;
    new_position.roll = Angle(Vector2(tn_.y(), tn_.z()), Vector2(nn.y(), nn.z()));
    /*Matrix4 matrix = Matrix4::Translate(tp_.x, tp_.y, tp_.z) * Matrix4::RotateX(new_position.roll) *
        Matrix4::RotateY(new_position.pitch) * Matrix4::RotateZ(new_position.yaw) * Matrix4::Translate(-tp_.x, -tp_.y, -tp_.z);
    myfloat tx, ty, tz;
    matrix.ToTranslate(tx, ty, tz);
    new_position.x = np.x + tx;
    new_position.y = np.y + ty;
    new_position.z = np.z + tz;*/

    lock->set_position(new_position);
    tn_ = lock->positioned_polygon_normals()[traced->triangle_index];
    context()->window->set_grab_input(true);
    rotate_ = true;
  }
}

bool WeldTransform::ProcessKeyDown(const SDL_KeyboardEvent &event) {
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
      ProcessEnter();
      break;
    case SDLK_g:
      if (rotate_)
        return true;
    case SDLK_KP8:
      move_state_.cam_up = 1;
      if (rotate_)
        return true;
      break;
    case SDLK_KP2:
      move_state_.cam_down = -1;
      if (rotate_)
        return true;
      break;
    case SDLK_KP4:
      move_state_.cam_left = 1;
      if (rotate_)
        return true;
      break;
    case SDLK_KP6:
      move_state_.cam_right = -1;
      if (rotate_)
        return true;
      break;
    default:
      break;
  }
  return false;
}

bool WeldTransform::ProcessKeyUp(const SDL_KeyboardEvent &event) {
  switch (event.keysym.sym) {
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

void WeldTransform::PostRenderStep() {
  if (finish_) return;

  auto lock = object_.lock();
  if (!lock) {
    context()->window->UnregisterWorker(this);
    return;
  }

  if (rotate_) {
    myfloat rotation_k_ = M_PI_2 / (context()->window->height() * rotation_speed_);

    char cam_left_right = move_state_.cam_left + move_state_.cam_right;
    char cam_up_down = move_state_.cam_up + move_state_.cam_down;
    xrel_ += cam_left_right * 10;
    yrel_ += cam_up_down * 10;
    myfloat dist = sqrt(Sqr(xrel_) + Sqr(yrel_));
    if (dist != 0) {
      myfloat angle = dist * rotation_k_;

      Position new_position(lock->position());
      Vector3 u = new_position.GetRotateMatrixTo() * tn_;

      myfloat roll, pitch, yaw;
      /*u.AxisToEuler(angle, roll, pitch, yaw);
      Matrix4 matrix = Matrix4::Translate(tp_.x, tp_.y, tp_.z) * Matrix4::RotateX(roll) *
          Matrix4::RotateY(pitch) * Matrix4::RotateZ(yaw) * Matrix4::Translate(-tp_.x, -tp_.y, -tp_.z);
      //matrix.ToRotate(roll, pitch, yaw);
      myfloat tx, ty, tz;
      matrix.ToTranslate(tx, ty, tz);
      /*new_position.x += tx;
      new_position.y += ty;
      new_position.z += tz;*/
      new_position.roll += roll;
      new_position.pitch += pitch;
      new_position.yaw += yaw;

      lock->set_position(new_position);
    }
  }

  if (wait_finish_) {
    context()->window->UnregisterWorker(this);
    finish_ = true;
  }

  xrel_ = 0;
  yrel_ = 0;
}

void WeldTransform::set_rotation_speed(const myfloat rotation_speed) {
  rotation_speed_ = rotation_speed;
}
