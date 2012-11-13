#include "cachedtextcontrol.h"

CachedTextControl::CachedTextControl() = default;

void CachedTextControl::set_font(const sdlobj::Font &font) {
  TextControl::set_font(font);
  Invalidate();
}

void CachedTextControl::set_font_color(const sdlobj::Color &font_color) {
  TextControl::set_font_color(font_color);
  Invalidate();
}

void CachedTextControl::set_back_color(const sdlobj::Color &back_color) {
  TextControl::set_back_color(back_color);
  Invalidate();
}
