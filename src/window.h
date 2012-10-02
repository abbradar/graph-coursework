#ifndef GRAPH_WINDOW_H_
#define GRAPH_WINDOW_H_

#include "sdlobj/sdl.h"
#include "sdlobj/font.h"
#include "sdlobj/color.h"
#include "sdlobj/frametimer.h"
#include "windowlogdestination.h"
#include "interface.h"
#include "rasterizer.h"
#include "position.h"
#include "scene.h"

class Window {
 public:
  Window(int width, int height, int bpp, int fps);
  ~Window();

  static const Uint32 kSDLSubsystems;

  const char *caption();
  void set_caption(const char *name);

  inline const sdlobj::Font &font() {
    return font_;
  }

  inline const sdlobj::Color &font_color() {
    return font_color_;
  }

  void set_font(const sdlobj::Font &font);
  void set_font_color(const sdlobj::Color &font_color);

  float fps();
  void set_fps(float fps);

  bool show_fps();
  void set_show_fps(bool show_fps);

  int show_fps_rate();
  void set_show_fps_rate(int show_fps_rate);

  int width();
  int height();
  int bpp();

  inline Interface *interface() {
    return interface_;
  }

  inline Position *position() {
    return position_;
  }

  inline Scene *scene() {
    return scene_;
  }

  // TODO: ugly, fix this
  inline Rasterizer &rasterizer() {
    return rasterizer_;
  }

  void SetVideoMode(int width, int height, int bpp);

  void Run();

 private:
  static const Uint32 kVideoModeFlags;

  sdlobj::Font font_;
  sdlobj::Color font_color_;
  sdlobj::FrameTimer frame_timer_;
  WindowLogDestination *window_log_;
  Position *position_;
  Interface *interface_;
  Rasterizer rasterizer_;
  Scene *scene_;
  int fps_step_;
};

#endif // GRAPH_COURSEWINDOW_H_
