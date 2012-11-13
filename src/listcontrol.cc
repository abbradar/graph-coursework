#include <boost/format.hpp>
#include "common/math.h"
#include "common/debug.h"
#include "listcontrol.h"

using namespace sdlobj;
using namespace std;

ListControl::ListControl() : style_(kSimple), selected_(-1) {}

void ListControl::set_items(const std::vector<string> &items) {
  items_ = items;
  selected_ = -1;
  Invalidate();
}

void ListControl::Repaint(Surface &surface) {
  Uint32 font_p = surface.ColorToPixel(font_color());
  Uint32 back_p = surface.ColorToPixel(back_color());
  surface.Fill(back_p);
  unsigned curr_y = 0;
  unsigned line_skip = font().line_skip();
  for (size_t i = 0; i < items_.size(); ++i) {
    string str;
    switch (style_) {
      case kSimple:
        str = items_[i];
        break;
      case kEnumerated:
        str = (boost::format("{1}: {2}") % (i + 1) % items_[i]).str();
        break;
      default:
        Assert(false);
    }
    if ((int)i == selected_) {
      Surface rendered = font().RenderUTF8_Blended(str.data(), back_color());
      surface.FillRect(0, curr_y, width(), line_skip, font_p);
      surface.Blit(rendered, 0, curr_y);
    } else {
      Surface rendered = font().RenderUTF8_Blended(str.data(), font_color());
      surface.Blit(rendered, 0, curr_y);
    }
    curr_y += line_skip;
  }
}

void ListControl::MouseButton(const SDL_MouseButtonEvent &event) {
  unsigned line_skip = font().line_skip();

  set_selected(event.y / line_skip);
}

void ListControl::OnSelected() {}

void ListControl::set_style(const Style style) {
  if (style != style_) {
    style_ = style;
    Invalidate();
  }
}

void ListControl::set_selected(const int selected) {
  if (selected_ != selected) {
    selected_ = selected;
    Invalidate();
    OnSelected();
  }
}

unsigned ListControl::preferred_height() {
  return font().line_skip() * items_.size();
}

unsigned ListControl::preferred_width() {
  unsigned max = 0;

  for (size_t i = 0; i < items_.size(); ++i) {
    string str;
    switch (style_) {
      case kSimple:
        str = items_[i];
        break;
      case kEnumerated:
        str = (boost::format("{1}: {2}") % (i + 1) % items_[i]).str();
        break;
      default:
        Assert(false);
    }
    unsigned w;
    font().SizeUTF8(str.data(), &w, nullptr);
    if (w > max) max = w;
  }
  return max;
}
