#include <boost/format.hpp>
#include <SDL.h>
#include "logging.h"
#include "coursework.h"

using namespace std;

int CourseWork::Run(int argc, const char **argv) {
  logging::Logger::instance().set_name("Course Work");
#if DEBUG_LEVEL == 4
  logging::Logger::instance().set_level(logging::kDebug);
#endif

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    LogCritical((boost::format("Unable to init SDL: %1%") % SDL_GetError()).str().data());
  }
  LogDebug("SDL initialized");
  return 0;
}

void CourseWork::Terminate(int exit_code) noexcept {
}
