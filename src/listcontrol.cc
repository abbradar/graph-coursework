#include <boost/format.hpp>
#include "common/math.h"
#include "common/debug.h"
#include "listcontrol.h"

using namespace sdlobj;
using namespace std;

ListControl::ListControl() : style_(kSimple), selected_(-1), active_(-1) {}

void ListControl::set_items(const std::vector<string> &items) {
  items_ = items;
  selected_ = -1;
  active_ = -1;
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
    if ((int)i == active_) {
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

bool ListControl::ProcessMouseButtonDown(const SDL_MouseButtonEvent &event) {
  if (active_ != -1) {
    set_selected(active_);
    return true;
  } else return false;
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

bool ListControl::ProcessMouseMotion(const SDL_MouseMotionEvent &event) {
  int new_active;
  if (InBounds((unsigned)event.x, x(), x() + width()) &&
      InBounds((unsigned)event.y, y(), y() + height())) {
    new_active = (event.y - y()) / font().line_skip();
  } else {
    new_active = -1;
  }
  if (new_active != active_) {
    active_ = new_active;
    Invalidate();
  }
  return true;
}
