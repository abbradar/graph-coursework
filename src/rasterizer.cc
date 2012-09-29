#include <stdexcept>
#include "rasterizer.h"

using namespace std;
using namespace sdlobj;

// TODO: fix values
const float Rasterizer::kAOVX = 2.0f;
const float Rasterizer::kAOVY = 2.0f;
const float Rasterizer::kRenderDistance = 1.0f;

Rasterizer::Rasterizer() : scene_(nullptr), camera_(nullptr),
  surface_(nullptr), z_buffer_size_(0) {}

void Rasterizer::Render() {
  // fairly fast for ~60 ops and adds safety
  int img_size = surface_->width() * surface_->height();
  if (z_buffer_size_ != img_size) {
    z_buffer_.reset(new float[img_size]);
    z_buffer_size_ = img_size;
  }
  if (!(scene_ && camera_ && surface_)) throw runtime_error("Set scene, camera and surface");
  float *z_buf = this->z_buffer_.get();
}

void Rasterizer::set_scene(Scene *scene) {
  scene_ = scene;
}

void Rasterizer::set_camera(Position *camera) {
  camera_ = camera;
}

void Rasterizer::set_surface(Surface *surface) {
  surface_ = surface;
}
