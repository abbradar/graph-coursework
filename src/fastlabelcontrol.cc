#include "fastlabelcontrol.h"

using namespace sdlobj;

FastLabelControl::FastLabelControl() : x_(0), y_(0) {}

FastLabelControl::~FastLabelControl() = default;

void FastLabelControl::Paint(sdlobj::Surface &surface) {
  Surface rendered = font().RenderUTF8_Solid(label().data(), font_color());
  surface.Blit(rendered, x_, y_);
}

void FastLabelControl::set_position(unsigned x, unsigned y) {
  x_ = x;
  y_ = y;
}
