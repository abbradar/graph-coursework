#ifndef GRAPH_WINDOWLOGDESTINATION_H_
#define GRAPH_WINDOWLOGDESTINATION_H_

#include <deque>
#include <sstream>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include "sdlobj/surface.h"
#include "sdlobj/font.h"
#include "common/logging.h"

class WindowLogDestination : public logging::LogDestination {
 public:
  typedef boost::posix_time::time_facet TimeFacet;

  WindowLogDestination(const sdlobj::Font &font, SDL_Color color = { .r = 0xFF, .g = 0xFF, .b = 0xFF });
  ~WindowLogDestination();

  void WriteLog(logging::LogMessageLevel level, const char *line);

  inline const TimeFacet &time_facet() const noexcept {
    return *time_facet_;
  }

  void set_time_facet(TimeFacet *time_facet) noexcept;

  inline int max_items() {
    return max_items_;
  }

  void set_max_items(const unsigned int max_items);

  inline const sdlobj::Font &font() {
    return font_;
  }

  void set_font(const sdlobj::Font &font);

  inline SDL_Color color() {
    return color_;
  }

  void set_color(const SDL_Color color);

  sdlobj::Surface Render();

 private:
  struct WindowMessage {
    WindowMessage(const std::string &&other) : message(other) {}

    std::string message;
    sdlobj::Surface *surface = nullptr;
  };

  TimeFacet *time_facet_;
  sdlobj::Font font_;
  SDL_Color color_;
  std::stringstream time_format_;
  std::deque<WindowMessage> deque_;
  unsigned int max_items_ = 3;

  void ClearRendered();
};

#endif // WINDOWLOGDESTINATION_H
