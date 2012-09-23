#include <boost/format.hpp>
#include <memory>
#include "common/logging.h"
#include "common/debug.h"
#include "sdlobj/sdl.h"
#include "sdlobj/sdlttf.h"
#include "windowlogdestination.h"
#include "courseeventhandler.h"
#include "coursework.h"

using namespace std;
using namespace sdlobj;
using namespace logging;

int CourseWork::Run(int argc, const char **argv) {
  Logger::instance().set_name(kProgramName);
#if DEBUG_LEVEL == 4
  Logger::instance().set_level(logging::kDebug);
#endif

  SDL::instance().event_handler().reset(new CourseEventHandler());
  SDL::instance().Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  SDLTTF::instance().Init();

  default_font_ = font_manager_.GetFont(string(kDefaultFontName), kDefaultFontSize, kDefaultFontIndex);
  WindowLogDestination *window_log_ = new WindowLogDestination(default_font_);
  Logger::instance().destinations().push_back(Logger::LogDestinationPointer(window_log_));

  SDL::instance().SetVideoMode(640, 480, 32, SDL_ASYNCBLIT | SDL_HWACCEL | SDL_HWSURFACE | SDL_RESIZABLE | SDL_DOUBLEBUF);

  int timer = 0;

  while (true) {
    Uint32 last_time = SDL_GetTicks();
    ++timer;
    if (timer == 60) {
      LogDebug("60 frames passed");
      timer = 0;
    }
    while (SDL::instance().PollEvent());
    SDL::instance().surface().Fill(0x0);
    Surface log = window_log_->Render();
    SDL::instance().surface().Blit(log);
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
  SDLTTF::instance().Free();
  SDL::instance().Free();
}
