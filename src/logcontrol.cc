#include "logcontrol.h"

using namespace std;
using namespace sdlobj;

LogControl::LogControl() : buffer_(1) {}

void LogControl::AddItem(const std::string &line) {
  buffer_.push_front(LogMessage(line));
  Invalidate();
}

void LogControl::Clear() {
  buffer_.clear();
  Invalidate();
}

void LogControl::set_max_items(const unsigned int max_items) {
  if (max_items < buffer_.size()) {
    Invalidate();
  }
  buffer_.set_capacity(max_items);
}

unsigned LogControl::preferred_width() {
  unsigned max = 0;

  for (LogMessage &i : buffer_) {
    unsigned w;
    if (i.surface) {
      w = i.surface->width();
    } else {
      font().SizeUTF8(i.message.data(), &w, nullptr);
    }
    if (w > max) max = w;
  }
  return max;
}

unsigned LogControl::preferred_height() {
  return font_.line_skip() * max_items();
}

void LogControl::ClearRendered() {
  for (LogMessage &i : buffer_) {
    delete i.surface;
    i.surface = nullptr;
  }
  Invalidate();
}

void LogControl::Repaint(sdlobj::Surface &surface) {
  Uint32 back_p = surface.ColorToPixel(back_color_);
  surface.Fill(back_p);
  unsigned curr_y = 0;
  unsigned line_skip = font_.line_skip();
  for (auto i = buffer_.rbegin(); i != buffer_.rend(); ++i) {
    if (!i->surface) {
      i->surface = new Surface(font_.RenderUTF8_Blended(i->message.data(), font_color_));
    }
    surface.Blit(*i->surface, 0, curr_y);
    curr_y += line_skip;
  }
}

LogControl::LogMessage::LogMessage(const std::string &message_) : message(message_) {}

LogControl::LogMessage::~LogMessage() {
  delete surface;
}
