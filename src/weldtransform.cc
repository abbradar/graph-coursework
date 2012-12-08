#include "common/exception.h"
#include "common/math.h"
#include "vector2.h"
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

  tn_ = lock->model()->polygon_normals()[traced->triangle_index];
  tnl_ = (lock->model()->points()[lock->model()->polygons()[traced->triangle_index].points[0]]
      - tp_).normalized();
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

    const Vector3 &onn = -new_lock->positioned_polygon_normals()[traced->triangle_index];
    Vector3 nn = onn;
    Vector3 nnl = (new_lock->positioned_points()[
                   new_lock->model()->polygons()[traced->triangle_index].points[0]]
        - np).normalized();

    Position new_position;
    new_position.yaw = NormAngle(Vector2(nn.x(), nn.y()), Vector2(tn_.x(), tn_.y()));
    AffineTransform z_transform(RotateTransform(-new_position.yaw, Vector3::UnitZ()));
    AffineTransform r_z_transform(RotateTransform(new_position.yaw, Vector3::UnitZ()));
    nn = z_transform * nn;
    tn_ = r_z_transform * tn_;
    nnl = z_transform * nnl;
    tnl_ = r_z_transform * tnl_;
    new_position.pitch = -NormAngle(Vector2(nn.x(), nn.z()), Vector2(tn_.x(), tn_.z()));
    AffineTransform y_transform(RotateTransform(-new_position.pitch, Vector3::UnitY()));
    AffineTransform r_y_transform(RotateTransform(new_position.pitch, Vector3::UnitY()));
    nn = y_transform * nn;
    tn_ = r_y_transform * tn_;
    nnl = y_transform * nnl;
    tnl_ = r_y_transform * tnl_;
    new_position.roll = NormAngle(Vector2(nnl.y(), nnl.z()), Vector2(tnl_.y(), tnl_.z()));
    tp_ = new_position.GetMatrixFrom() * tp_;
    new_position.x = np.x() - tp_.x();
    new_position.y = np.y() - tp_.y();
    new_position.z = np.z() - tp_.z();

    lock->set_position(new_position);
    tn_ = onn;
    tp_ = np;
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

      const Position &old_position = lock->position();

      AffineTransform matrix = TranslateTransform(tp_.x(), tp_.y(), tp_.z())
          * RotateTransform(angle, tn_) * TranslateTransform(-tp_.x(), -tp_.y(), -tp_.z())
          * old_position.GetMatrixFrom();

      Position new_position;

      TransformToTranslate(matrix, new_position.x, new_position.y, new_position.z);
      TransformToEuler(matrix, new_position.roll, new_position.pitch, new_position.yaw);

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
