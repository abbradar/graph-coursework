#include "multilinelabel.h"

using namespace std;
using namespace sdlobj;

MultiLineLabel::MultiLineLabel() = default;

void MultiLineLabel::set_items(const std::vector<string> &items) {
  items_ = items;
  Invalidate();
}

void MultiLineLabel::Repaint(Surface &surface) {
  Uint32 back_p = surface.ColorToPixel(back_color());
  SetColorKey(SDL_SRCCOLORKEY, back_p);
  surface.Fill(back_p);
  unsigned curr_y = 0;
  unsigned line_skip = font().line_skip();
  for (size_t i = 0; i < items_.size(); ++i) {
    Surface rendered = font().RenderUTF8_Blended(items_[i].data(), font_color());
    surface.Blit(rendered, 0, curr_y);
    curr_y += line_skip;
  }
}

unsigned MultiLineLabel::preferred_height() {
  return font().line_skip() * items_.size();
}

unsigned MultiLineLabel::preferred_width() {
  unsigned max = 0;

  for (size_t i = 0; i < items_.size(); ++i) {
    unsigned w;
    font().SizeUTF8(items_[i].data(), &w, nullptr);
    if (w > max) max = w;
  }
  return max;
}


MultiLineLabel::~MultiLineLabel() = default;
