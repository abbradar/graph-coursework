#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "sdlobj/sdl.h"
#include "windowlogdestination.h"

using namespace std;
using namespace sdlobj;
using namespace boost::posix_time;

WindowLogDestination::WindowLogDestination(const Font &font, SDL_Color color) :
  time_facet_(new TimeFacet("%x %X")), font_(font), color_(color) {
  time_format_.imbue(locale(time_format_.getloc(), time_facet_));
}

WindowLogDestination::~WindowLogDestination() {
  delete time_facet_;
}

void WindowLogDestination::WriteLog(logging::LogMessageLevel level, const char *line) {
  if (max_items_ == 0) return; // more like sanity check

  boost::format a;
  stringstream ss;
  time_format_ << second_clock::local_time();
  ss << boost::format("%1% [%2%] %3%: %4%") % LevelName(level)[0] % time_format_.str() %
        logging::Logger::instance().name() % line;
  time_format_.str("");

  WindowMessage message(ss.str());

  while (deque_.size() > max_items_) deque_.pop_front();
  deque_.push_back(message);
}

void WindowLogDestination::set_time_facet(TimeFacet *time_facet) noexcept {
  delete time_facet_;
  time_facet_ = time_facet;
  time_format_.imbue(locale(time_format_.getloc(), time_facet_));
}

void WindowLogDestination::set_max_items(const unsigned int max_items) {
  max_items_ = max_items;
  while (deque_.size() > max_items_) deque_.pop_front();
}

void WindowLogDestination::set_font(const Font &font) {
  font_ = font;
  ClearRendered();
}

void WindowLogDestination::set_color(const SDL_Color color) {
  color_ = color;
  ClearRendered();
}

Surface WindowLogDestination::Render() {
  unsigned int max_width = 0;
  for (auto &i : deque_) {
    if (!i.surface)
      i.surface = new Surface(font_.RenderUTF8_Solid(i.message.data(), color_));
    max_width = max_width > i.surface->width() ? max_width : i.surface->width();
  }
  int line_skip = font_.line_skip();
  Surface surface(max_width, line_skip * max_items_);
  int y = 0;
  for (auto &i : deque_) {
    surface.Blit(*i.surface, 0, y);
    y += line_skip;
  }
  return surface;
}

void WindowLogDestination::ClearRendered() {
  for (auto &i : deque_) {
    delete i.surface;
    i.surface = nullptr;
  }
}
