#include <memory>
#include <boost/format.hpp>
#include "common/logging.h"
#include "common/debug.h"
#include "common/exception.h"
#include "sdlobj/sdlimage.h"
#include "fontsettings.h"
#include "colorsettings.h"
#include "windowsettings.h"
#include "modelssettings.h"
#include "scenesettings.h"
#include "window.h"

// DEBUG
#include <fstream>

using namespace std;
using namespace sdlobj;
using namespace logging;
using namespace boost;
using namespace settings;

const Uint32 Window::kVideoModeFlags = SDL_ASYNCBLIT | SDL_HWACCEL | SDL_HWSURFACE
    | SDL_RESIZABLE | SDL_DOUBLEBUF;

const Uint32 Window::kSDLSubsystems = SDL_INIT_VIDEO | SDL_INIT_TIMER;
const Uint32 Window::kSDLImageSubsystems = IMG_INIT_PNG;

Window::Window(int width, int height, int bpp, int fps) :
 frame_timer_(fps), window_log_(new WindowLogDestination()),
 position_(new Position()), interface_(new Interface(fps)), scene_(new Scene()),
 models_(new Models()), fps_step_(1), show_fps_(true), projected_height_(1) {
  Logger::instance().destinations().push_back(Logger::DestinationPointer(window_log_));
  settings_.AddBlock(std::shared_ptr<SettingsBlock>(new WindowSettings(this)));
  settings_.AddBlock(std::shared_ptr<SettingsBlock>(new ModelsSettings(models_)));
  settings_.AddBlock(std::shared_ptr<SettingsBlock>(new SceneSettings(scene_)));
  interface_->set_position(position_);
  frame_timer_.set_measure_fps(true);

  SDL::instance().event_handler().reset(interface_);
  rasterizer_.set_camera(position_);
  rasterizer_.set_scene(scene_);
  rasterizer_.set_trace(true);
  set_fps(fps);

  SetVideoMode(width, height, bpp);
}

Window::Window() : Window(640, 480, 32, 60) {}

Window::~Window() {
  for (auto i = Logger::instance().destinations().begin();
       i != Logger::instance().destinations().end(); ++i) {
    if (i->get() == window_log_) {
      Logger::instance().destinations().erase(i);
      break;
    }
  }
  delete scene_;
  delete models_;
  delete interface_;
  delete position_;
}

const char *Window::caption() {
  return SDL::instance().caption();
}

void Window::set_caption(const char * name) {
  SDL::instance().set_icon_caption(name);
  SDL::instance().set_caption(name);
}

void Window::set_font(const Font &font) {
  font_ = font;
  window_log_->set_font(font_);
}

void Window::set_font_color(const Color &font_color) {
  font_color_ = font_color;
  window_log_->set_color(font_color_);
}

float Window::fps() {
  return frame_timer_.fps();
}

void Window::set_fps(float fps) {
  frame_timer_.set_fps(fps);
}

bool Window::show_fps() {
  return show_fps_;
}

void Window::set_show_fps(bool show_fps) {
  show_fps_ = show_fps;
}

int Window::show_fps_rate() {
  return frame_timer_.measure_ticks();
}

void Window::set_show_fps_rate(int show_fps_rate) {
  frame_timer_.set_measure_ticks(show_fps_rate);
}

void Window::set_crosshair(const Surface &crosshair) {
  crosshair_ = crosshair;
}

int Window::width() {
  return SDL::instance().surface().width();
}

int Window::height() {
  return SDL::instance().surface().height();
}

int Window::bpp() {
  return SDL::instance().surface().bpp();
}

myfloat Window::viewer_distance() {
  return rasterizer_.viewer_distance();
}

void Window::set_viewer_distance(const myfloat viewer_distance) {
  rasterizer_.set_viewer_distance(viewer_distance);
}

myfloat Window::move_speed() {
  return interface_->move_speed();
}

void Window::set_move_speed(const myfloat move_speed) {
  interface_->set_move_speed(move_speed);
}

myfloat Window::rotation_speed() {
  return interface_->rotation_speed();
}

void Window::set_rotation_speed(const myfloat rotation_speed) {
  interface_->set_rotation_speed(rotation_speed);
}

void Window::set_projected_height(const myfloat projected_height) {
  if (projected_height <= 0)
    throw Exception("Projected height should be > 0");
  projected_height_ = projected_height;
}

void Window::SetVideoMode(const int width, const int height, const int bpp) {
  SDL::instance().SetVideoMode(width, height, bpp, kVideoModeFlags);
}

void Window::Run() {
  // DEBUG
  ifstream scene("scene.yml");
  LoadScene(*scene_, scene, *models_);
  scene.close();

  Surface fps_r;
  fps_step_ = 0;

  LogDebug("Starting event loop");

  while (frame_timer_.WaitFrame()) { // event loop
    // This can produce situation when loop will work infinitely
    // when events will add up faster than be produced,
    // but this is bad anyway, so let's try to deal with them all.
    while (SDL::instance().PollEvent()); // this is explicit exit point (we can receive quit event here)
    interface_->Step();
    Surface &screen = SDL::instance().surface();
    screen.Fill(0x000000);

    rasterizer_.set_surface(&screen);
    rasterizer_.set_scale(screen.height() / projected_height_);
    rasterizer_.set_trace_point(screen.width() / 2, screen.height() / 2);
    rasterizer_.FirstPass();
    std::shared_ptr<SceneObject> traced = rasterizer_.traced_object().lock();
    rasterizer_.SecondPass();

    screen.Blit(crosshair_, (screen.width() - crosshair_.width()) / 2,
                (screen.height() - crosshair_.height()) / 2);

    Surface log = window_log_->Render();
    screen.Blit(log, 2, 0);

    // TODO make set_show_position and stuff
    string position_str = (boost::format("Position: x: %1%, y: %2%, z: %3%, pitch: %4%, yaw: %5%")
                  % position_->x % position_->y
                  % position_->z % position_->pitch
                  % position_->yaw).str();
    Surface position_r = font_.RenderUTF8_Solid(position_str.data(), font_color_);
    position_r.SetColorKey(0, 0);
    screen.Blit(position_r, 2, screen.height() - font_.line_skip());

    if (fps_step_ == 0) {
      myfloat measured_fps = frame_timer_.measured_fps();
      interface_->set_fps(measured_fps);
      if (show_fps_) {
        string fps_str = (boost::format("%.1f fps") % measured_fps).str();
        fps_r = font_.RenderUTF8_Solid(fps_str.data(), font_color_);
      }
    }
    ++fps_step_;
    if (fps_step_ == frame_timer_.measure_ticks()) fps_step_ = 0;
    if (show_fps_) {
      screen.Blit(fps_r, screen.width() - fps_r.width() - 2,
                  screen.height() - font_.line_skip());
    }

    if (!interface_->grab_mouse()) {
      string mouse_str = (boost::format("x: %1%, y: %2%") % interface_->x() % interface_->y()).str();
      Surface mouse_r = font_.RenderUTF8_Solid(mouse_str.data(), font_color_);
      screen.Blit(mouse_r, screen.width() - mouse_r.width() - 2,
                  screen.height() - 2 * font_.line_skip());
    }

    SDL::instance().Flip();
  }

  AssertMsg(false, "Sudden exit from event loop!");
}

void Window::LoadSettings(istream &in) {
  settings_.LoadSettings(in);
}

void Window::SaveSettings(ostream &out) {
  settings_.SaveSettings(out);
}
