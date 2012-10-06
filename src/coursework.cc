#include <boost/format.hpp>
#include <memory>
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
const char *const kDefaultFontName = "DroidSans.ttf";
const int kDefaultFontIndex = 0;
const int kDefaultFontSize = 12;
const Color kDefaultFontColor = Color(0xFF, 0xFF, 0xFF);
const int kDefaultWidth = 640;
const int kDefaultHeight = 480;
const int kDefaultBpp = 32;
const float kDefaultFps = 60;
const bool kDefaultShowFps = true;
const int kDefaultFpsMeasureRate = 10;
const myfloat kDefaultMoveSpeed = 120;
const myfloat kDefaultRotationSpeed = 1.0 / 3.0;
const myfloat kDefaultViewerDistance = 1000;
const myfloat kDefaultProjectedHeight = 200;

CourseWork::CourseWork() = default;

CourseWork::~CourseWork() = default;

int CourseWork::Run(int argc, const char **argv) {
  Logger::instance().set_name(kProgramName);
#if DEBUG_LEVEL == 4
  Logger::instance().set_level(logging::kDebug);
#endif

  SDL::instance().Init(Window::kSDLSubsystems);
  SDLTTF::instance().Init();

  Font font = Font(kDefaultFontName, kDefaultFontSize, kDefaultFontIndex);

  Window *window = new Window(kDefaultWidth, kDefaultHeight, kDefaultBpp, kDefaultFps);
  window->set_caption(kProgramName);
  window->set_font(font);
  window->set_font_color(kDefaultFontColor);
  window->set_move_speed(kDefaultMoveSpeed);
  window->set_rotation_speed(kDefaultRotationSpeed);
  window->set_show_fps(kDefaultShowFps);
  window->set_show_fps_rate(kDefaultFpsMeasureRate);
  window->set_viewer_distance(kDefaultViewerDistance);
  window->set_projected_height(kDefaultProjectedHeight);

  // placeholder
  window->scene()->Load();

  LogDebug("Initialization complete");

  // we'll never return from this one
  window->Run();

  delete window;

  return 0;
}

void CourseWork::Terminate(int exit_code) noexcept {
  LogDebug((boost::format("Terminating, exit code: %1%") % exit_code).str().data());
}
