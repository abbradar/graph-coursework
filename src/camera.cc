#include "common/exception.h"
#include "camera.h"

Camera::Camera() : viewer_distance_(1), scale_(1), width_(0), height_(0), k_(0) {}

void Camera::set_size(const myfloat width, const myfloat height) {
  if (width <= 0 || height <= 0) {
    throw Exception("Width and height should be positive.");
  }
  width_ = width;
  height_ = height;
  k_ = viewer_distance_ * height_ * scale_;
}

void Camera::set_viewer_distance(const myfloat viewer_distance) {
  if (viewer_distance <= 0) {
    throw Exception("Viewer distance should be positive.");
  }
  viewer_distance_ = viewer_distance;
  k_ = viewer_distance_ * height_ * scale_;
}

void Camera::set_scale(const myfloat scale) {
  if (scale <= 0) {
    throw Exception("Scale should be positive.");
  }
  scale_ = scale;
  k_ = viewer_distance_ * height_ * scale_;
}

void Camera::PerspectiveTransform(Point3D &point) const {
  myfloat kk = k_ / (viewer_distance_ + point.z);
  point.x = point.x * kk + width_ / 2.0;
  point.y = point.y * kk + height_ / 2.0;
}

void Camera::ReversePerspectiveTransform(Point3D &point) const {
  myfloat kk = (viewer_distance_ + point.z) / k_;
  point.x = (point.x - width_ / 2.0) * kk;
  point.y = (point.y - height_ / 2.0) * kk;
}
