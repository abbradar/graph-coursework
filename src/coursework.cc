#include <fstream>
#include <memory>
#include <boost/format.hpp>
#include "common/logging.h"
#include "common/debug.h"
#include "sdlobj/sdl.h"
#include "sdlobj/sdlttf.h"
#include "window.h"
#include "coursework.h"

using namespace std;
using namespace sdlobj;
using namespace logging;

const char *const CourseWork::kProgramName = "Course Work";
const char *const kSettingsFileName = "config.yml";

CourseWork::CourseWork() = default;

CourseWork::~CourseWork() = default;

int CourseWork::Run(int argc, const char **argv) {
  Logger::instance().set_name(kProgramName);
#if DEBUG_LEVEL == 4
  Logger::instance().set_level(logging::kDebug);
#else
  Logger::instance().set_level(logging::kWarning);
#endif

  SDL::instance().Init(Window::kSDLSubsystems);
  SDLTTF::instance().Init();

  Window *window = new Window();

  ifstream settings(kSettingsFileName);
  window->LoadSettings(settings);
  settings.close();
  LogNotice("Initialization complete");

  // we'll never return from this one
  window->Run();
  Assert(false);

  delete window;

  return EXIT_SUCCESS;
}

void CourseWork::Terminate(int exit_code) noexcept {
  LogNotice((boost::format("Terminating, exit code: %1%") % exit_code).str().data());
}
