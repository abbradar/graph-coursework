#ifndef GRAPH_DRAWINGWINDOW_H_
#define GRAPH_DRAWINGWINDOW_H_

#include <SDL_video.h>

/** Handles setting video modes and drawing. */
class DrawingWindow {
 public:
  DrawingWindow() = default;
  DrawingWindow(const DrawingWindow &other) = delete;

  void SetVideoMode(unsigned int x, unsigned int y, unsigned int bpp);

  void StartDrawing();
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

  void EventLoop();

 private:
  SDL_Surface *surface_;
  Uint32 (*get_pixel_)(Uint8 *) = nullptr;
  void (*set_pixel_)(Uint8 *, Uint32) = nullptr;
};

#endif // GRAPH_DRAWINGWINDOW_H_
