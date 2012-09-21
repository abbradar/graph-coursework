#include <boost/format.hpp>
#include <SDL.h>
#include "logging.h"
#include "coursework.h"

using namespace std;

const char *kProgramName = "Course Work";

int CourseWork::Run(int argc, const char **argv) {
  logging::Logger::instance().set_name(kProgramName);
#if DEBUG_LEVEL == 4
  logging::Logger::instance().set_level(logging::kDebug);
#endif

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    LogCritical((boost::format("Unable to init SDL: %1%") % SDL_GetError()).str().data());
    return 1;
  }
  LogDebug("SDL initialized");

  window_.SetVideoMode(640, 480, 32);

  window_.StartDrawing();
  for (int i = 10; i < 100; ++i) {
    for (int j = 10; j < 100; ++j) {
      window_.SetPixel(i, j, 0xFFFFFF);
    }
  }
  window_.EndDrawing();

  SDL_Delay(-1);

  LogDebug("Exiting normally");
  return 0;
}

void CourseWork::Terminate(int exit_code) noexcept {
  LogDebug("Terminating application");
  SDL_Quit();
}
