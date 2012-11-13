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
 frame_timer_(fps), context_(new Context(this)),
 log_control_destination_(new LogControlDestination()) {
  logging::Logger::instance().destinations().push_back(log_control_destination_);
  settings_.AddBlock(std::shared_ptr<SettingsBlock>(new WindowSettings(this)));
  settings_.AddBlock(std::shared_ptr<SettingsBlock>(new ModelsSettings(&context_->models)));
  frame_timer_.set_measure_fps(true);

  set_fps(fps);

  SetVideoMode(width, height, bpp);

  fps_label_ = make_shared<FPSLabel>(context_);
  rasterizer_ = make_shared<Rasterizer>(context_);
  position_handler_ = make_shared<PositionHandler>(context_);
  log_control_ = make_shared<LogControl>();
  position_label_ = make_shared<PositionLabel>(context_);
  pointer_drawer_ = make_shared<PointerDrawer>(context_);
  mouse_position_label_ = make_shared<MousePositionLabel>(context_);

  log_control_destination_->set_log_control(log_control_);

  RegisterWorker(static_pointer_cast<ConveyorWorker>(log_control_));
  RegisterWorker(static_pointer_cast<ConveyorWorker>(fps_label_));
  RegisterWorker(static_pointer_cast<ConveyorWorker>(position_handler_));
  RegisterWorker(static_pointer_cast<ConveyorWorker>(rasterizer_));
  RegisterWorker(static_pointer_cast<ConveyorWorker>(position_label_));
  RegisterWorker(static_pointer_cast<ConveyorWorker>(pointer_drawer_));
  RegisterWorker(static_pointer_cast<ConveyorWorker>(mouse_position_label_));
}

Window::Window() : Window(640, 480, 32, 60) {}

Window::~Window() {
  for (auto i = Logger::instance().destinations().begin();
       i != Logger::instance().destinations().end(); ++i) {
    if (*i == log_control_destination_) {
      Logger::instance().destinations().erase(i);
      break;
    }
  }
}

struct ActiveCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessActive(std::forward<Args>(args)...);
  }
};

void Window::ProcessActive(const SDL_ActiveEvent &event) {
  ProcessEvent<SDL_ActiveEvent, ActiveCaller>(event);
}

struct KeyDownCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessKeyDown(std::forward<Args>(args)...);
  }
};

void Window::ProcessKeyDown(const SDL_KeyboardEvent &event) {
  ProcessEvent<SDL_KeyboardEvent, KeyDownCaller>(event);
}

struct KeyUpCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessKeyUp(std::forward<Args>(args)...);
  }
};

void Window::ProcessKeyUp(const SDL_KeyboardEvent &event) {
  ProcessEvent<SDL_KeyboardEvent, KeyUpCaller>(event);
}

struct MouseMotionCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessMouseMotion(std::forward<Args>(args)...);
  }
};

void Window::ProcessMouseMotion(const SDL_MouseMotionEvent &event) {
  cursor_x_ = event.x;
  cursor_y_ = event.y;
  ProcessEvent<SDL_MouseMotionEvent, MouseMotionCaller>(event);
}

struct MouseButtonDownCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessMouseButtonDown(std::forward<Args>(args)...);
  }
};

void Window::ProcessMouseButtonDown(const SDL_MouseButtonEvent &event) {
  ProcessEvent<SDL_MouseButtonEvent, MouseButtonDownCaller>(event);
}

struct MouseButtonUpCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessMouseButtonUp(std::forward<Args>(args)...);
  }
};

void Window::ProcessMouseButtonUp(const SDL_MouseButtonEvent &event) {
  ProcessEvent<SDL_MouseButtonEvent, MouseButtonUpCaller>(event);
}

struct JoyAxisCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessJoyAxis(std::forward<Args>(args)...);
  }
};

void Window::ProcessJoyAxis(const SDL_JoyAxisEvent &event) {
  ProcessEvent<SDL_JoyAxisEvent, JoyAxisCaller>(event);
}

struct JoyBallCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessJoyBall(std::forward<Args>(args)...);
  }
};

void Window::ProcessJoyBall(const SDL_JoyBallEvent &event) {
  ProcessEvent<SDL_JoyBallEvent, JoyBallCaller>(event);
}

struct JoyHatCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessJoyHat(std::forward<Args>(args)...);
  }
};

void Window::ProcessJoyHat(const SDL_JoyHatEvent &event) {
  ProcessEvent<SDL_JoyHatEvent, JoyHatCaller>(event);
}

struct JoyButtonDownCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessJoyButtonDown(std::forward<Args>(args)...);
  }
};

void Window::ProcessJoyButtonDown(const SDL_JoyButtonEvent &event) {
  ProcessEvent<SDL_JoyButtonEvent, JoyButtonDownCaller>(event);
}

struct JoyButtonUpCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessJoyButtonUp(std::forward<Args>(args)...);
  }
};

void Window::ProcessJoyButtonUp(const SDL_JoyButtonEvent &event) {
  ProcessEvent<SDL_JoyButtonEvent, JoyButtonUpCaller>(event);
}

struct ResizeCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessResize(std::forward<Args>(args)...);
  }
};

void Window::ProcessResize(const SDL_ResizeEvent &event) {
  SetVideoMode(event.h, event.w, bpp());
  ProcessEvent<SDL_ResizeEvent, ResizeCaller>(event);
}

struct ExposeCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessExpose(std::forward<Args>(args)...);
  }
};

void Window::ProcessExpose(const SDL_ExposeEvent &event) {
  ProcessEvent<SDL_ExposeEvent, ExposeCaller>(event);
}

struct QuitCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessQuit(std::forward<Args>(args)...);
  }
};

void Window::ProcessQuit(const SDL_QuitEvent &event) {
  ProcessEvent<SDL_QuitEvent, QuitCaller>(event);
  LogDebug("Exit event received; exiting...");
  exit(EXIT_SUCCESS);
}

struct UserCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessUser(std::forward<Args>(args)...);
  }
};

void Window::ProcessUser(const SDL_UserEvent &event) {
  ProcessEvent<SDL_UserEvent, UserCaller>(event);
}

struct SysWMCaller {
  template <class... Args> static bool Call(EventWorker *receiver, Args &&... args) {
    return receiver->ProcessSysWM(std::forward<Args>(args)...);
  }
};

void Window::ProcessSysWM(const SDL_SysWMEvent &event) {
  ProcessEvent<SDL_SysWMEvent, SysWMCaller>(event);
}

template<class Event, class Handler> void Window::ProcessEvent(const Event &event) {
  for (auto &i: workers_) {
    if (i->event_worker()) {
      if (Handler::Call(i->event_worker(), event)) break;
    }
  }
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
  fps_label_->set_font(font_);
  log_control_->set_font(font_);
  position_label_->set_font(font_);
  mouse_position_label_->set_font(font_);
}

void Window::set_font_color(const Color &font_color) {
  font_color_ = font_color;
  fps_label_->set_font_color(font_color_);
  log_control_->set_font_color(font_color_);
  position_label_->set_font_color(font_color_);
  mouse_position_label_->set_font_color(font_color_);
}

void Window::set_back_color(const Color &back_color) {
  back_color_ = back_color;
  fps_label_->set_back_color(back_color_);
  log_control_->set_back_color(back_color_);
  position_label_->set_back_color(back_color_);
  mouse_position_label_->set_back_color(back_color_);
}

float Window::fps() {
  return frame_timer_.fps();
}

void Window::set_fps(float fps) {
  frame_timer_.set_fps(fps);
}

float Window::measured_fps() {
  return frame_timer_.measured_fps();
}

bool Window::show_fps() {
  return fps_label_->show();
}

void Window::set_show_fps(bool show_fps) {
  fps_label_->set_show(show_fps);
}

int Window::show_fps_rate() {
  return frame_timer_.measure_ticks();
}

void Window::set_show_fps_rate(int show_fps_rate) {
  frame_timer_.set_measure_ticks(show_fps_rate);
  fps_label_->set_fps_step(show_fps_rate);
}

bool Window::grab_input() {
  return SDL::instance().grab_input();
}

void Window::set_grab_input(const bool grab_input) {
  SDL::instance().set_grab_input(grab_input);
}

const sdlobj::Surface &Window::pointer() {
  return pointer_drawer_->pointer();
}

void Window::set_pointer(const Surface &pointer) {
  pointer_drawer_->set_pointer(pointer);
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
  return rasterizer_->viewer_distance();
}

void Window::set_viewer_distance(const myfloat viewer_distance) {
  rasterizer_->set_viewer_distance(viewer_distance);
}

myfloat Window::move_speed() {
  return position_handler_->move_speed();
}

void Window::set_move_speed(const myfloat move_speed) {
  position_handler_->set_move_speed(move_speed);
}

myfloat Window::rotation_speed() {
  return position_handler_->rotation_speed();
}

void Window::set_rotation_speed(const myfloat rotation_speed) {
  position_handler_->set_rotation_speed(rotation_speed);
}

myfloat Window::scale() {
  return rasterizer_->scale();
}

void Window::set_scale(const myfloat scale) {
  rasterizer_->set_scale(scale);
}

void Window::SetVideoMode(const int width, const int height, const int bpp) {
  SDL::instance().SetVideoMode(width, height, bpp, kVideoModeFlags);
}

void Window::Run() {
  LogDebug("Starting event loop");

  while (frame_timer_.WaitFrame()) { // event loop
    workers_.splice(workers_.begin(), new_workers_);
    for (auto &i : remove_workers_) {
      for (auto j = workers_.begin(); j != workers_.end(); ++j) {
        if (j->get() == i) {
          j = workers_.erase(j);
        }
      }
    }
    remove_workers_.clear();

    // This can produce situation when loop will work infinitely
    // when events will add up faster than be produced,
    // but this is bad anyway, so let's try to deal with them all.
    while (SDL::instance().PollEvent()); // this is explicit exit point (we can receive quit event here)

    fps_label_->set_position(0, width() - fps_label_->preferred_width());

    Surface &screen = SDL::instance().surface();
    screen.Fill(screen.ColorToPixel(back_color_));
    rasterizer_->Prepare();

    for (auto &i: workers_) {
      if (i->pre_render_worker()) {
        i->pre_render_worker()->PreRenderStep();
      }
    }
    for (auto i = workers_.rbegin(); i != workers_.rend(); ++i) {
      if ((*i)->render_worker()) {
        (*i)->render_worker()->Paint(screen);
      }
    }
    for (auto &i: workers_) {
      if (i->post_render_worker()) {
        i->post_render_worker()->PostRenderStep();
      }
    }

    SDL::instance().Flip();
  }

  AssertMsg(false, "Sudden exit from event loop!");
}

void Window::RegisterWorker(const std::shared_ptr<ConveyorWorker> &worker) {
  new_workers_.push_front(worker);
}

void Window::UnregisterWorker(const ConveyorWorker *worker) {
  remove_workers_.push_back(worker);
}

void Window::LoadSettings(istream &in) {
  settings_.LoadSettings(in);
}

void Window::SaveSettings(ostream &out) {
  settings_.SaveSettings(out);
}

bool Window::show_cursor() {
  return SDL::instance().show_cursor();
}

void Window::set_show_cursor(const bool show_cursor) {
  SDL::instance().set_show_cursor(show_cursor);
}
