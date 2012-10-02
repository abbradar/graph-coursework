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

const char * const CourseWork::kProgramName = "Course Work";
const char * const kDefaultFontName = "DroidSans.ttf";
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
const myfloat kDefaultViewerDistance = 100;

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
  window->interface()->set_move_speed(kDefaultMoveSpeed);
  window->interface()->set_rotation_speed(kDefaultRotationSpeed);
  window->set_show_fps(kDefaultShowFps);
  window->set_show_fps_rate(kDefaultFpsMeasureRate);
  window->rasterizer().set_viewer_distance(kDefaultViewerDistance);

  // test object
  Point3D test_points[] = { Point3D(0, 0, 0), Point3D(100, 0, 0), Point3D(0, 0, 100),
                            Point3D(0, 100, 0), Point3D(100, 100, 0), Point3D(0, 100, 100),
                            Point3D(100, 0, 100), Point3D(100, 100, 100) };
  IndexedTriangle test_indexes[] = { IndexedTriangle(0, 1, 2), IndexedTriangle(1, 2, 6),
                                    IndexedTriangle(0, 1, 3), IndexedTriangle(1, 3, 4),
                                    IndexedTriangle(0, 2, 3), IndexedTriangle(2, 3, 5),
                                    IndexedTriangle(7, 6, 5), IndexedTriangle(6, 5, 2),
                                    IndexedTriangle(7, 6, 4), IndexedTriangle(6, 4, 1),
                                    IndexedTriangle(7, 5, 4), IndexedTriangle(0, 1, 3) };
  PointVector points(test_points, test_points + 8);
  TriangleVector polygons(test_indexes, test_indexes + 12);
  SceneObject object(points, polygons, Position(0, 0, 0));
  object.color() = Color(0xFF, 0xFF, 0xFF);
  window->scene()->objects().push_back(std::move(object));

  LogDebug("Initialization complete");

  // we'll never return from this one
  window->Run();

  delete window;

  return 0;
}

void CourseWork::Terminate(int exit_code) noexcept {
  LogDebug((boost::format("Terminating, exit code: %1%") % exit_code).str().data());
}
