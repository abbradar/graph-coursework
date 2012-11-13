#include "labelcontrol.h"

LabelControl::LabelControl() : valid_width_(false), width_(0) {}

void LabelControl::set_label(const std::string &label) {
  label_ = label;
  valid_width_ = false;
}

void LabelControl::set_font(const sdlobj::Font &font) {
  TextControl::set_font(font);
  valid_width_ = false;
}

unsigned LabelControl::preferred_height() {
  return font().line_skip();
}

unsigned LabelControl::preferred_width() {
  if (!valid_width_) {
    font().SizeUTF8(label_.data(), &width_, nullptr);
    valid_width_ = true;
  }
  return width_;
}
