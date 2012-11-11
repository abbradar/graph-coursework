#ifndef GRAPH_WINDOW_H_
#define GRAPH_WINDOW_H_

#include <iostream>
#include "common/settings.h"
#include "sdlobj/sdl.h"
#include "sdlobj/font.h"
#include "sdlobj/color.h"
#include "sdlobj/frametimer.h"
#include "windowlogdestination.h"
#include "interface.h"
#include "rasterizer.h"
#include "position.h"
#include "scene.h"
#include "models.h"

class Window {
 public:
  Window(int width, int height, int bpp, int fps);
  Window();
  ~Window();

  static const Uint32 kSDLSubsystems;
  static const Uint32 kSDLImageSubsystems;

  const char *caption();
  void set_caption(const char * name);

  inline const sdlobj::Font &font() {
    return font_;
  }

  inline const sdlobj::Color &font_color() {
    return font_color_;
  }

  void set_font(const sdlobj::Font &font);
  void set_font_color(const sdlobj::Color &font_color);

  float fps();
  void set_fps(const float fps);

  bool show_fps();
  void set_show_fps(const bool show_fps);

  int show_fps_rate();
  void set_show_fps_rate(const int show_fps_rate);

  inline const sdlobj::Surface &crosshair() {
    return crosshair_;
  }

  void set_crosshair(const sdlobj::Surface &crosshair);

  int width();
  int height();
  int bpp();

  inline Position *const position() {
    return position_;
  }

  inline Scene *const scene() {
    return scene_;
  }

  myfloat viewer_distance();
  void set_viewer_distance(const myfloat viewer_distance);

  myfloat move_speed();
  void set_move_speed(const myfloat move_speed);

  myfloat rotation_speed();
  void set_rotation_speed(const myfloat rotation_speed);

  /** Coordinate range which is projected on screen height */
  inline myfloat projected_height() {
    return projected_height_;
  }

  void set_projected_height(const myfloat projected_height);

  void SetVideoMode(const int width, const int height, const int bpp);

  void Run();

  void LoadSettings(std::istream &in);
  void SaveSettings(std::ostream &out);

 private:
  static const Uint32 kVideoModeFlags;

  sdlobj::Font font_;
  sdlobj::Color font_color_;
  sdlobj::FrameTimer frame_timer_;
  sdlobj::Surface crosshair_;
  WindowLogDestination *window_log_;
  Position *position_;
  Interface *interface_;
  Rasterizer rasterizer_;
  Scene *scene_;
  Models *models_;
  Settings settings_;
  unsigned int fps_step_;
  bool show_fps_;
  myfloat projected_height_;
};

#endif // GRAPH_COURSEWINDOW_H_
