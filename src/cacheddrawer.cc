#include "cacheddrawer.h"

using namespace sdlobj;

CachedDrawer::CachedDrawer() : x_(0), y_(0), width_(preferred_width()),
  height_(preferred_height()), valid_(false), surface_(width_, height_) {}

CachedDrawer::~CachedDrawer() = default;

void CachedDrawer::set_position(const unsigned x, const unsigned y) {
  x_ = x;
  y_ = y;
}

void CachedDrawer::set_size(const unsigned width, const unsigned height) {
  width_ = width;
  height_ = height;
  surface_ = Surface(width_, height_);
  Invalidate();
}

unsigned CachedDrawer::preferred_width() {
  return 0;
}

unsigned CachedDrawer::preferred_height() {
  return 0;
}

void CachedDrawer::Invalidate() {
  valid_ = false;
}

void CachedDrawer::Paint(Surface &surface) {
  if (!valid_) {
    valid_ = true;
    Repaint(surface_);
  }
  surface.Blit(surface_, x_, y_);
}
