#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "sdlobj/sdl.h"
#include "windowlogdestination.h"

using namespace std;
using namespace sdlobj;
using namespace boost::posix_time;

WindowLogDestination::WindowLogDestination() :
  WindowLogDestination(Font(), Color(0xFF, 0xFF, 0xFF)) {}

WindowLogDestination::WindowLogDestination(const Font &font, Color color) :
  time_facet_(new TimeFacet("%x %X")), font_(font), color_(color), max_items_(3) {
  time_format_.imbue(locale(time_format_.getloc(), time_facet_));
}

WindowLogDestination::~WindowLogDestination() {
  //delete time_facet_;
}

void WindowLogDestination::WriteLog(const logging::LogMessageLevel level, const char *line) {
  if (max_items_ == 0) return; // more like sanity check

  boost::format a;
  stringstream ss;
  time_format_ << second_clock::local_time();
  ss << boost::format("%1% [%2%] %3%: %4%") % LevelName(level)[0] % time_format_.str() %
        logging::Logger::instance().name() % line;
  time_format_.str("");

  WindowMessage message(ss.str());

  while (buffer_.size() > max_items_ - 1) buffer_.pop_front();
  buffer_.push_back(std::move(message));
}

void WindowLogDestination::set_time_facet(TimeFacet *const time_facet) noexcept {
  delete time_facet_;
  time_facet_ = time_facet;
  time_format_.imbue(locale(time_format_.getloc(), time_facet_));
}

void WindowLogDestination::set_max_items(const unsigned int max_items) {
  max_items_ = max_items;
  while (buffer_.size() > max_items_) buffer_.pop_front();
}

void WindowLogDestination::set_font(const Font &font) {
  font_ = font;
  ClearRendered();
}

void WindowLogDestination::set_color(const Color &color) {
  color_ = color;
  ClearRendered();
}

Surface WindowLogDestination::Render() {
  unsigned int max_width = 0;
  for (auto &i : buffer_) {
    if (!i.surface)
      i.surface.reset(new Surface(font_.RenderUTF8_Blended(i.message.data(), color_)));
    max_width = max_width > i.surface->width() ? max_width : i.surface->width();
  }
  int line_skip = font_.line_skip();
  Surface surface(max_width, line_skip * max_items_);
  surface.SetColorKey(SDL_SRCCOLORKEY | SDL_RLEACCEL, 0x0);
  int y = 0;
  for (auto &i : buffer_) {
    surface.Blit(*i.surface, 0, y);
    y += line_skip;
  }
  return surface;
}

void WindowLogDestination::ClearRendered() {
  for (auto &i : buffer_) {
    i.surface.reset(nullptr);
  }
}
