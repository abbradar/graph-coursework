#ifndef SDLOBJ_SURFACEPAINTER_H_
#define SDLOBJ_SURFACEPAINTER_H_

#include "common/debug.h"
#include "common/exception.h"
#include "surface.h"

#if DEBUG_LEVEL == 4
#include <stdexcept>
#include <boost/format.hpp>
#endif

namespace sdlobj {

class SurfacePainter {
 public:
  SurfacePainter();
  explicit SurfacePainter(Surface *surface);

  inline Surface *surface() {
    return surface_;
  }

  void set_surface(Surface *surface);

  inline void StartDrawing() {
    surface_->Lock();
  }

  inline void FinishDrawing() {
    surface_->Unlock();
  }

  /** Get the pixel at (x, y) */
  inline Uint32 GetPixel(const unsigned int x, const unsigned int y) {
#if DEBUG_LEVEL == 4
    if (x >= (unsigned int)surface_struct_->w || y >= (unsigned int)surface_struct_->h) {
      throw Exception((boost::format("Coordinates out of bounds: %1%,%2%") % x % y).str());
    }
#endif
    Uint8 *p = (Uint8 *)surface_struct_->pixels + y * surface_struct_->pitch
        + x * surface_struct_->format->BytesPerPixel;

    return get_pixel_(p);
  }

  /** Set the pixel at (x, y) to the given value */
  inline void SetPixel(const unsigned int x, const unsigned int y, const Uint32 pixel) {
#if DEBUG_LEVEL == 4
    if (x >= (unsigned int)surface_struct_->w || y >= (unsigned int)surface_struct_->h) {
      throw Exception((boost::format("Coordinates out of bounds: %1%, %2%") % x % y).str());
    }
#endif
    Uint8 *p = (Uint8 *)surface_struct_->pixels + y * surface_struct_->pitch
        + x * surface_struct_->format->BytesPerPixel;

    set_pixel_(p, pixel);
  }

  /** Draws line (integer Bresenham's algorithm) */
  void DrawLine(const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2,
                const Uint32 pixel);

 private:
  Surface *surface_;
  SDL_Surface *surface_struct_;

  Uint32 (*get_pixel_)(const Uint8 *);
  void (*set_pixel_)(Uint8 *, const Uint32);
};

}

#endif // SDLOBJ_SURFACEPAINTER_H_
