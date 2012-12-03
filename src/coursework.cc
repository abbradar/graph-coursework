#include <fstream>
#include <memory>
#include <boost/format.hpp>
#include "common/logging.h"
#include "common/debug.h"
#include "sdlobj/sdl.h"
#include "sdlobj/sdlttf.h"
#include "sdlobj/sdlimage.h"
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
#if DEBUG_LEVEL >= 3
  Logger::instance().set_level(logging::kDebug);
#else
  Logger::instance().set_level(logging::kWarning);
#endif

  SDL::instance().Init(Window::kSDLSubsystems);
  SDLTTF::instance().Init();
  SDLImage::instance().Init(Window::kSDLImageSubsystems);

  shared_ptr<Window> window = make_shared<Window>();

  ifstream settings(kSettingsFileName);
  window->LoadSettings(settings);
  settings.close();
  SDL::instance().event_handler() = window;
  LogNotice("Initialization complete");

  // we'll never return from this one
  window->Run();

  Assert(false);
  return EXIT_FAILURE;
}

void CourseWork::Terminate(int exit_code) noexcept {
  LogNotice((boost::format("Terminating, exit code: %1%") % exit_code).str().data());
}
