#include "common/math.h"
#include "point2d.h"
#include "movetransform.h"

const Point3D MoveTransform::kP0 = Point3D(0, 0, 0);
const Point3D MoveTransform::kP1 = Point3D(1, 0, 0);
const Point3D MoveTransform::kP2 = Point3D(0, 1, 0);

MoveTransform::MoveTransform(const std::shared_ptr<Context> &context, const std::shared_ptr<SceneObject> &object) :
 ContextUser(context), xrel_(0), yrel_(0), object_(object), wait_finish_(false), finish_(false),
  rotate_(false), rotation_speed_(1) {
  context->window->set_grab_input(true);
  auto lock = object_.lock();

  old_position_ = lock->position();
  //distance_ = old_position_->z;

  Matrix4 obj_transform = context->camera.GetMatrixTo() * old_position_.GetMatrixFrom();
  p0_ = obj_transform * kP0;
  p1_ = obj_transform * kP1;
  p2_ = obj_transform * kP2;
}

bool MoveTransform::ProcessMouseMotion(const SDL_MouseMotionEvent &event) {
  if (!rotate_) return false;
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
      xrel_ = 0;
      yrel_ = 0;
      rotate_ = true;
      return true;
    case SDLK_g:
      return true;
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
  /*static const Point3D p0 = Point3D(0, 0, 0);
  static const Point3D p1 = Point3D(1, 0, 0);
  static const Point3D p2 = Point3D(0, 1, 0);

  Position &my_new_position = context()->position;
  Position &obj_old_position = object_->position();

  Matrix4 transform = obj_old_position.GetMatrixTo() * my_old_position_.GetMatrixFrom() *
      Matrix4::RotateZ(my_new_position.yaw - my_old_position_.yaw) *
      Matrix4::RotateY(my_new_position.pitch - my_old_position_.pitch) *
      Matrix4::Translate(my_new_position.x - my_old_position_.x,
                         my_new_position.y - my_old_position_.y,
                         my_new_position.z - my_old_position_.z) *
      my_old_position_.GetMatrixTo() * obj_old_position.GetMatrixFrom();
  Point3D tp0 = transform * p0,
      tp1 = transform * p2,
      tp2 = transform * p2;
  Position new_position;
  new_position.x = obj_old_position.x + tp0.x;
  new_position.y = obj_old_position.y + tp0.y;
  new_position.z = obj_old_position.z + tp0.z;
  tp1 -= tp0; tp2 -= tp0;

  myfloat z_la = tp1.y / Point3D::Distance(tp0, tp1);
  myfloat z_a = asin(z_la);
  if (tp1.x < 0) {
    z_a = M_PI - z_a;
  }
  new_position.yaw = Circle(z_a + obj_old_position.yaw, -M_PI_2, M_PI_2);*/

  // move part
  if (rotate_) {
    /*myfloat rotation_k_ = M_PI_2 / (context()->window->height() * rotation_speed_);
    Position obj_old_position = object_->position();
    obj_old_position.pitch += yrel_ * rotation_k_;
    obj_old_position.pitch = Circle<myfloat>(obj_old_position.pitch, -M_PI_2, M_PI_2);
    obj_old_position.yaw += xrel_ * rotation_k_;
    obj_old_position.yaw = Circle<myfloat>(obj_old_position.yaw, -M_PI, M_PI);
    object_->set_position(obj_old_position);

    yrel_ = 0;
    xrel_ = 0;*/
  } else {
    /*Point3D new_position = Point3D(distance_, 0, 0);
    new_position = context()->position.GetMatrixFrom() * new_position;
    object_->set_position(Position(new_position.x, new_position.y, new_position.z,
                                   object_->position().roll, object_->position().pitch,
                                   object_->position().yaw));*/

    Matrix4 from_camera = context()->camera.GetMatrixFrom();
    Point3D n_p0 = from_camera * p0_,
            n_p1 = from_camera * p1_,
            n_p2 = from_camera * p2_;
    Position new_position;
    new_position.x = n_p0.x;
    new_position.y = n_p0.y;
    new_position.z = n_p0.z;
    n_p1 -= n_p0;
    n_p2 -= n_p0;
    /*Matrix4 tr_transform = new_position.GetTranslateMatrixTo();
    n_p1 = tr_transform * n_p1;
    n_p2 = tr_transform * n_p2;*/
    new_position.yaw = Point2D(n_p1.x, n_p1.y).Angle();
    Matrix4 z_transform = Matrix4::RotateZ(-new_position.yaw);
    n_p1 = z_transform * n_p1;
    n_p2 = z_transform * n_p2;
    new_position.pitch = Point2D(n_p1.x, n_p1.z).Angle();
    /*if (new_position.yaw > M_PI_2 && new_position.yaw < M_PI + M_PI_2) {
      new_position.pitch = -new_position.pitch;
    }*/
    Matrix4 y_transform = Matrix4::RotateY(-new_position.pitch);
    //n_p1 = y_transform * n_p1;
    n_p2 = y_transform * n_p2;
    //n_p2 = new_position.GetRotateMatrixTo() * n_p2;
    new_position.roll = Point2D(n_p2.y, n_p2.z).Angle();
    /*Matrix4 x_transform = Matrix4::RotateX(-new_position.roll);
    n_p1 = x_transform * n_p1;
    n_p2 = x_transform * n_p2;*/
    /*new_position.roll = Point2D(n_p1.y, n_p1.z).Angle() - M_PI_2;
    new_position.roll = Circle<myfloat>(new_position.roll, 0, 2 * M_PI);
    new_position.pitch = Point2D(n_p1.x, n_p1.z).Angle() - M_PI_2;
    new_position.pitch = Circle<myfloat>(new_position.pitch, 0, 2 * M_PI);
    if (new_position.roll > M_PI_2 && new_position.roll < M_PI + M_PI_2) {
      new_position.pitch = -new_position.pitch;
    }
    n_p2 = new_position.GetRotateMatrixTo() * n_p2;
    new_position.yaw = Point2D(n_p2.x, n_p2.y).Angle();*/

    object_.lock()->set_position(new_position);
  }

  if (wait_finish_) {
    context()->window->UnregisterWorker(this);
    finish_ = true;
  }
}

void MoveTransform::set_rotation_speed(const myfloat rotation_speed) {
  rotation_speed_ = rotation_speed;
}
