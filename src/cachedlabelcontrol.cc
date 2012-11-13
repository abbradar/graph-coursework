#include "cachedlabelcontrol.h"

CachedLabelControl::CachedLabelControl() = default;

void CachedLabelControl::set_label(const std::string &label) {
  LabelControl::set_label(label);
  Invalidate();
}

void CachedLabelControl::set_font(const sdlobj::Font &font) {
  LabelControl::set_font(font);
  Invalidate();
}


unsigned CachedLabelControl::preferred_height() {
  return LabelControl::preferred_height();
}

unsigned CachedLabelControl::preferred_width() {
  return LabelControl::preferred_width();
}
