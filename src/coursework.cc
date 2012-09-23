#include <boost/format.hpp>
#include <memory>
#include "common/logging.h"
#include "common/debug.h"
#include "sdlobj/sdl.h"
#include "sdlobj/sdlttf.h"
#include "courseinterface.h"
#include "windowlogdestination.h"
#include "coursework.h"

using namespace std;
using namespace sdlobj;
using namespace logging;

const SDL_Color CourseWork::kDefaultFontColor = { .r = 0xFF, .g = 0xFF, .b = 0xFF };

int CourseWork::Run(int argc, const char **argv) {
  Logger::instance().set_name(kProgramName);
#if DEBUG_LEVEL == 4
  Logger::instance().set_level(logging::kDebug);
#endif

  WindowLogDestination *window_log_ = new WindowLogDestination();
  Logger::instance().destinations().push_back(Logger::DestinationPointer(window_log_));

  SDL::instance().Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  SDLTTF::instance().Init();

  default_font_ = font_manager_.GetFont(string(kDefaultFontName), kDefaultFontSize, kDefaultFontIndex);

  window_log_->set_font(default_font_);
  window_log_->set_color(kDefaultFontColor);

  SDL::instance().SetVideoMode(640, 480, 32, SDL_ASYNCBLIT | SDL_HWACCEL | SDL_HWSURFACE | SDL_RESIZABLE | SDL_DOUBLEBUF);

  SDL::instance().set_caption(kProgramName);
  SDL::instance().set_icon_caption(kProgramName);

  CourseInterface *interface_ = new CourseInterface();
  SDL::instance().event_handler().reset(interface_);

  int timer = 0;

  while (true) {
    Uint32 last_time = SDL_GetTicks();
    ++timer;
    if (timer == 60) {
      LogDebug("60 frames passed");
      timer = 0;
    }
    while (SDL::instance().PollEvent());
    interface_->Step();
    SDL::instance().surface().Fill(0x0);
    Surface log = window_log_->Render();
    SDL::instance().surface().Blit(log, 2, 0);
    string str = (boost::format("Position: x: %1%, y: %2%, y: %3%, p: %4%")
                  % interface_->position().x % interface_->position().y
                  % interface_->position().yaw % interface_->position().pitch).str();
    Surface position = default_font_.RenderUTF8_Solid(str.data(), kDefaultFontColor);
    SDL::instance().surface().Blit(position, 2, SDL::instance().surface().height() - default_font_.line_skip());
    SDL::instance().Flip();
    int time = tick_ - (int)(SDL_GetTicks() - last_time);
    if (time > 0) {
      SDL_Delay(time);
    }
  }

  AssertMsg(false, "Sudden exit from event loop!");
  return 1;
}

void CourseWork::Terminate(int exit_code) noexcept {
  LogDebug((boost::format("Terminating, exit code: %1%") % exit_code).str().data());
}
