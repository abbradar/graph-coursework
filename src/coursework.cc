#include <boost/format.hpp>
#include <memory>
#include "common/logging.h"
#include "common/debug.h"
#include "sdlobj/sdl.h"
#include "sdlobj/sdlttf.h"
#include "interface.h"
#include "coursework.h"
#include "scene.h"
#include "rasterizer.h"
#include "windowlogdestination.h"

using namespace std;
using namespace sdlobj;
using namespace logging;

const char * const CourseWork::kProgramName = "Course Work";
const char * const CourseWork::kDefaultFontName = "DroidSans.ttf";
const int CourseWork::kDefaultFontIndex = 0;
const int CourseWork::kDefaultFontSize = 12;
const Color CourseWork::kDefaultFontColor = Color(0xFF, 0xFF, 0xFF);
const int CourseWork::kFps = 60;
const int CourseWork::kWidth = 640;
const int CourseWork::kHeight = 480;
const int CourseWork::kBpp = 32;

CourseWork::CourseWork() = default;

CourseWork::~CourseWork() = default;

int kmsecsInSec = 1000;
float kmsecsInSecF = 1000.0f;

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

  SDL::instance().SetVideoMode(kWidth, kHeight, kBpp, SDL_ASYNCBLIT | SDL_HWACCEL | SDL_HWSURFACE | SDL_RESIZABLE | SDL_DOUBLEBUF);

  SDL::instance().set_caption(kProgramName);
  SDL::instance().set_icon_caption(kProgramName);

  Position *position = new Position();
  Interface *interface = new Interface();
  interface->set_position(position);
  SDL::instance().event_handler().reset(interface);

  Scene *scene = new Scene();

  Rasterizer *rasterizer = new Rasterizer();
  rasterizer->set_camera(position);
  rasterizer->set_scene(scene);
  rasterizer->set_surface(&SDL::instance().surface());

  LogDebug("Initialization complete");

  int tick = kmsecsInSec / kFps;
  float error = kmsecsInSecF / kFps - tick;
  float curr_error = 0; // this is for more stable FPS

  const int kFpsRate = 10; // rate of FPS measurement
  int sum_time = kFpsRate * kmsecsInSec / kFps; // this should produce initial fps=kFps
  int fps_step = 0;
  Surface fps_r;

  LogDebug("Starting event loop");

  while (true) { // event loop
    Uint32 last_time = SDL_GetTicks();
    // This can produce situation when loop will work infinitely
    // when events will add up faster than be produced,
    // but this is bad anyway, so let's try to deal with them all.
    while (SDL::instance().PollEvent()); // this is explicit exit point (we can receive quit event here)
    interface->Step();
    SDL::instance().surface().Fill(0x000000);
    rasterizer->Render();
    Surface log = window_log_->Render();
    SDL::instance().surface().Blit(log, 2, 0);
    string position_str = (boost::format("Position: x: %1%, y: %2%, z: %3%, pitch: %4%, yaw: %5%")
                  % position->x % position->y
                  % position->z % position->pitch
                  % position->yaw).str();
    Surface position_r = default_font_.RenderUTF8_Solid(position_str.data(), kDefaultFontColor);
    SDL::instance().surface().Blit(position_r, 2, SDL::instance().surface().height() - default_font_.line_skip());
    if (fps_step == 0) {
      float fps = kmsecsInSecF * kFpsRate / sum_time;
      sum_time = 0;
      string fps_str = (boost::format("%.1f fps") % fps).str();
      fps_r = default_font_.RenderUTF8_Solid(fps_str.data(), kDefaultFontColor);
    }
    ++fps_step;
    if (fps_step == kFpsRate) fps_step = 0;
    SDL::instance().surface().Blit(fps_r, SDL::instance().surface().width() - fps_r.width() - 2,
                                   SDL::instance().surface().height() - default_font_.line_skip());
    SDL::instance().Flip();
    int delay_time = 0;
    curr_error += error;
    if (curr_error >= 1.0f) {
      delay_time = 1;
      curr_error -= 1.0f;
    }
    Uint32 curr_time = SDL_GetTicks();
    int frame_time = curr_time - last_time;
    delay_time += tick - frame_time;
    if (delay_time > 0) {
      sum_time += frame_time + delay_time;
      SDL_Delay(delay_time);
    } else {
      sum_time += frame_time;
    }
  }

  AssertMsg(false, "Sudden exit from event loop!");
  return 1;
}

void CourseWork::Terminate(int exit_code) noexcept {
  LogDebug((boost::format("Terminating, exit code: %1%") % exit_code).str().data());
}
