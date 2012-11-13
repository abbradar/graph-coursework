#include "textcontrol.h"

TextControl::TextControl() : font_color_(255, 255, 255), back_color_(0, 0, 0) {}

TextControl::~TextControl() = default;

void TextControl::set_font(const sdlobj::Font &font) {
  font_ = font;
}

void TextControl::set_font_color(const sdlobj::Color &font_color) {
  font_color_ = font_color;
}

void TextControl::set_back_color(const sdlobj::Color &back_color) {
  back_color_ = back_color;
}
