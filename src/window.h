#ifndef GRAPH_WINDOW_H_
#define GRAPH_WINDOW_H_

#include <iostream>
#include <memory>
#include <list>
#include "common/settings.h"
#include "sdlobj/sdl.h"
#include "sdlobj/font.h"
#include "sdlobj/color.h"
#include "sdlobj/frametimer.h"
#include "sdlobj/eventhandler.h"
#include "context.h"
#include "conveyorworker.h"
#include "rasterizer.h"
#include "fpslabel.h"
#include "logcontroldestination.h"
#include "pointerdrawer.h"
#include "positionhandler.h"
#include "positionlabel.h"
#include "mousepositionlabel.h"

class PositionLabel;
class MousePositionLabel;
class PositionHandler;
class PointerDrawer;
class Rasterizer;
class FPSLabel;

class Window : public sdlobj::EventHandler {
 public:
  Window(int width, int height, int bpp, int fps);
  Window();
  ~Window();

  static const Uint32 kSDLSubsystems;
  static const Uint32 kSDLImageSubsystems;

  virtual void ProcessActive(const SDL_ActiveEvent &event);
  virtual void ProcessKeyDown(const SDL_KeyboardEvent &event);
  virtual void ProcessKeyUp(const SDL_KeyboardEvent &event);
  virtual void ProcessMouseMotion(const SDL_MouseMotionEvent &event);
  virtual void ProcessMouseButtonDown(const SDL_MouseButtonEvent &event);
  virtual void ProcessMouseButtonUp(const SDL_MouseButtonEvent &event);
  virtual void ProcessJoyAxis(const SDL_JoyAxisEvent &event);
  virtual void ProcessJoyBall(const SDL_JoyBallEvent &event);
  virtual void ProcessJoyHat(const SDL_JoyHatEvent &event);
  virtual void ProcessJoyButtonDown(const SDL_JoyButtonEvent &event);
  virtual void ProcessJoyButtonUp(const SDL_JoyButtonEvent &event);
  virtual void ProcessResize(const SDL_ResizeEvent &event);
  virtual void ProcessExpose(const SDL_ExposeEvent &event);
  virtual void ProcessQuit(const SDL_QuitEvent &event);
  virtual void ProcessUser(const SDL_UserEvent &event);
  virtual void ProcessSysWM(const SDL_SysWMEvent &event);

  const char *caption();
  void set_caption(const char * name);

  inline const sdlobj::Font &font() {
    return font_;
  }

  void set_font(const sdlobj::Font &font);

  inline const sdlobj::Color &font_color() {
    return font_color_;
  }

  void set_font_color(const sdlobj::Color &font_color);

  inline const sdlobj::Color &back_color() {
    return back_color_;
  }

  void set_back_color(const sdlobj::Color &back_color);

  float fps();
  void set_fps(const float fps);

  float measured_fps();

  bool show_fps();
  void set_show_fps(const bool show_fps);

  int show_fps_rate();
  void set_show_fps_rate(const int show_fps_rate);

  bool grab_input();
  void set_grab_input(const bool grab_input);

  bool show_cursor();
  void set_show_cursor(const bool show_cursor);

  int width();
  int height();
  int bpp();

  inline unsigned cursor_x() {
    return cursor_x_;
  }

  inline unsigned cursor_y() {
    return cursor_y_;
  }

  myfloat viewer_distance();
  void set_viewer_distance(const myfloat viewer_distance);

  myfloat move_speed();
  void set_move_speed(const myfloat move_speed);

  myfloat rotation_speed();
  void set_rotation_speed(const myfloat rotation_speed);

  myfloat scale();

  const sdlobj::Surface &pointer();
  void set_pointer(const sdlobj::Surface &pointer);

  void set_scale(const myfloat scale);

  void SetVideoMode(const int width, const int height, const int bpp);

  void Run();

  void RegisterWorker(const std::shared_ptr<ConveyorWorker> &worker);
  void UnregisterWorker(const ConveyorWorker *worker);

  void LoadSettings(std::istream &in);
  void SaveSettings(std::ostream &out);

 private:
  template<class Event, class Handler> void ProcessEvent(const Event &event);

  static const Uint32 kVideoModeFlags;

  unsigned cursor_x_;
  unsigned cursor_y_;
  sdlobj::Font font_;
  sdlobj::Color font_color_;
  sdlobj::Color back_color_;
  sdlobj::FrameTimer frame_timer_;
  std::shared_ptr<Context> context_;
  std::shared_ptr<FPSLabel> fps_label_;
  std::shared_ptr<Rasterizer> rasterizer_;
  std::shared_ptr<PositionHandler> position_handler_;
  std::shared_ptr<LogControl> log_control_;
  std::shared_ptr<LogControlDestination> log_control_destination_;
  std::shared_ptr<PointerDrawer> pointer_drawer_;
  std::shared_ptr<PositionLabel> position_label_;
  std::shared_ptr<MousePositionLabel> mouse_position_label_;
  Settings settings_;

  std::list<std::shared_ptr<ConveyorWorker>> workers_;
  std::list<std::shared_ptr<ConveyorWorker>> new_workers_;
  std::list<const ConveyorWorker *> remove_workers_;
};

#endif // GRAPH_COURSEWINDOW_H_
