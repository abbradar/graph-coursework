#include <stdexcept>
#include <boost/format.hpp>
#include "common/logging.h"
#include "common/debug.h"
#include "window.h"

using namespace std;
using namespace sdlobj;
using namespace logging;
using namespace boost;

const Uint32 Window::kVideoModeFlags = SDL_ASYNCBLIT | SDL_HWACCEL | SDL_HWSURFACE
    | SDL_RESIZABLE | SDL_DOUBLEBUF;

const Uint32 Window::kSDLSubsystems = SDL_INIT_VIDEO | SDL_INIT_TIMER;

Window::Window(int width, int height, int bpp, int fps) :
 frame_timer_(fps), window_log_(new WindowLogDestination()),
 position_(new Position()), interface_(new Interface(fps)), scene_(new Scene()) {
  Logger::instance().destinations().push_back(Logger::DestinationPointer(window_log_));
  SetVideoMode(width, height, bpp);
  interface_->set_position(position_);
  SDL::instance().event_handler().reset(interface_);
  rasterizer_.set_camera(position_);
  rasterizer_.set_scene(scene_);
  set_fps(fps);
}

Window::~Window() {
  for (auto i = Logger::instance().destinations().begin();
       i != Logger::instance().destinations().end(); ++i) {
    if (i->get() == window_log_) {
      Logger::instance().destinations().erase(i);
      break;
    }
  }
  delete scene_;
  delete interface_;
  delete position_;
}

const char *Window::caption() {
  return SDL::instance().caption();
}

void Window::set_caption(const char *name) {
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
  interface_->set_fps(fps);
}

bool Window::show_fps() {
  return frame_timer_.measure_fps();
}

void Window::set_show_fps(bool show_fps) {
  frame_timer_.set_measure_fps(show_fps);
  if (!show_fps) fps_step_ = 0;
}

int Window::show_fps_rate() {
  return frame_timer_.measure_ticks();
}

void Window::set_show_fps_rate(int show_fps_rate) {
  frame_timer_.set_measure_ticks(show_fps_rate);
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

void Window::SetVideoMode(int width, int height, int bpp) {
  SDL::instance().SetVideoMode(width, height, bpp, kVideoModeFlags);
}

void Window::Run() {
  Surface fps_r;
  fps_step_ = 0;

  LogDebug("Starting event loop");

  while (frame_timer_.WaitFrame()) { // event loop
    // This can produce situation when loop will work infinitely
    // when events will add up faster than be produced,
    // but this is bad anyway, so let's try to deal with them all.
    while (SDL::instance().PollEvent()); // this is explicit exit point (we can receive quit event here)
    interface_->Step();
    SDL::instance().surface().Fill(0x000000);
    rasterizer_.set_surface(&SDL::instance().surface());
    rasterizer_.Render();
    Surface log = window_log_->Render();
    SDL::instance().surface().Blit(log, 2, 0);
    // TODO make set_show_position and stuff
    string position_str = (boost::format("Position: x: %1%, y: %2%, z: %3%, pitch: %4%, yaw: %5%")
                  % position_->x % position_->y
                  % position_->z % position_->pitch
                  % position_->yaw).str();
    Surface position_r = font_.RenderUTF8_Solid(position_str.data(), font_color_);
    SDL::instance().surface().Blit(position_r, 2, SDL::instance().surface().height() - font_.line_skip());
    if (frame_timer_.measure_fps()) {
      if (fps_step_ == 0) {
        string fps_str = (boost::format("%.1f fps") % frame_timer_.measured_fps()).str();
        fps_r = font_.RenderUTF8_Solid(fps_str.data(), font_color_);
      }
      ++fps_step_;
      if (fps_step_ == frame_timer_.measure_ticks()) fps_step_ = 0;
      SDL::instance().surface().Blit(fps_r, SDL::instance().surface().width() - fps_r.width() - 2,
                                     SDL::instance().surface().height() - font_.line_skip());
    }
    SDL::instance().Flip();
  }

  AssertMsg(false, "Sudden exit from event loop!");
}