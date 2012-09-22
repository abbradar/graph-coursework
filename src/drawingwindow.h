#ifndef GRAPH_DRAWINGWINDOW_H_
#define GRAPH_DRAWINGWINDOW_H_

#include <SDL.h>

/** Handles setting video modes and drawing. */
class DrawingWindow {
 public:
  DrawingWindow() = default;
  DrawingWindow(const DrawingWindow &other) = delete;

  /** Set desired video mode and bits per pixel */
  void SetVideoMode(unsigned int x, unsigned int y, unsigned int bpp);

  /** Lock surface and prepare it for drawing */
  void StartDrawing();
  /** Unlock surface and flip buffers */
  void EndDrawing();

  /** Get the pixel at (x, y) */
  inline Uint32 GetPixel(unsigned int x, unsigned int y) {
    int bpp = surface_->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface_->pixels + y * surface_->pitch + x * bpp;

    return get_pixel_(p);
  }

  /** Set the pixel at (x, y) to the given value */
  inline void SetPixel(unsigned int x, unsigned int y, Uint32 pixel) {
    int bpp = surface_->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to set */
    Uint8 *p = (Uint8 *)surface_->pixels + y * surface_->pitch + x * bpp;

    set_pixel_(p, pixel);
  }

  /** Converts pixel to RGB format */
  inline void PixelToRGB(const Uint32 &pixel, Uint8 *r, Uint8 *g, Uint8 *b) {
    SDL_GetRGB(pixel, surface_->format, r, g, b);
  }

  /** Converts RGB to pixel according to screen format */
  inline Uint32 RGBToPixel(const Uint8 r, const Uint8 g, const Uint8 b) {
    return SDL_MapRGB(surface_->format, r, g, b);
  }

  /** Wait for events in the queue and process them */
  void WaitEvent();
  /** Immediately process events in the queue */
  bool PollEvent();

  virtual void ProcessActive(const SDL_ActiveEvent &) {}
  virtual void ProcessKeyDown(const SDL_KeyboardEvent &) {}
  virtual void ProcessKeyUp(const SDL_KeyboardEvent &) {}
  virtual void ProcessMouseMotion(const SDL_MouseMotionEvent &) {}
  virtual void ProcessMouseButtonDown(const SDL_MouseButtonEvent &) {}
  virtual void ProcessMouseButtonUp(const SDL_MouseButtonEvent &) {}
  virtual void ProcessJoyAxis(const SDL_JoyAxisEvent &) {}
  virtual void ProcessJoyBall(const SDL_JoyBallEvent &) {}
  virtual void ProcessJoyHat(const SDL_JoyHatEvent &) {}
  virtual void ProcessJoyButtonDown(const SDL_JoyButtonEvent &) {}
  virtual void ProcessJoyButtonUp(const SDL_JoyButtonEvent &) {}
  virtual void ProcessResize(const SDL_ResizeEvent &) {}
  virtual void ProcessExpose(const SDL_ExposeEvent &) {}
  virtual void ProcessQuit(const SDL_QuitEvent &) { exit(0); }
  virtual void ProcessUser(const SDL_UserEvent &) {}
  virtual void ProcessSysWM(const SDL_SysWMEvent &) {}

 protected:
  /** Implement this to set additional SDL flags for video mode */
  virtual int video_mode_flags() { return 0; } /* SDL_RESIZABLE | SDL_ASYNC_BLIT */

 private:
  SDL_Surface *surface_;
  Uint32 (*get_pixel_)(Uint8 *) = nullptr;
  void (*set_pixel_)(Uint8 *, Uint32) = nullptr;

  void ProcessEvent(const SDL_Event &event);
};

#endif // GRAPH_DRAWINGWINDOW_H_
