#include <boost/format.hpp>
#include <memory>
#include <SDL.h>
#include "logging.h"
#include "debug.h"
#include "filelogdestination.h"
#include "coursework.h"

using namespace std;

const char *kProgramName = "Course Work";

int CourseWork::Run(int argc, const char **argv) {
  logging::Logger::instance().set_name(kProgramName);
#if DEBUG_LEVEL == 4
  logging::Logger::instance().set_level(logging::kDebug);
#endif
  // Windows don't have stderr for GUI applications ;_;
  FileLogDestination *dest = new FileLogDestination("out.log");
  logging::Logger::instance().destinations().push_back(dest);

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

  while (true) {
    window_.WaitEvent();
  }

  AssertMsg(false, "Sudden exit from event loop!");
  return 1;
}

void CourseWork::Terminate(int exit_code) noexcept {
  LogDebug((boost::format("Terminating, exit code: %1%") % exit_code).str().data());
  SDL_Quit();
}
