#ifndef SDLOBJ_SURFACEPAINTER_H_
#define SDLOBJ_SURFACEPAINTER_H_

#include "surface.h"

namespace sdlobj {

class SurfacePainter {
 public:
  SurfacePainter() = default;
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
  inline Uint32 GetPixel(unsigned int x, unsigned int y) {
    int bpp = surface_->surface()->format->BytesPerPixel;

    Uint8 *p = surface_->pixels() + y * surface_->surface()->pitch + x * bpp;

    return get_pixel_(p);
  }

  /** Set the pixel at (x, y) to the given value */
  inline void SetPixel(unsigned int x, unsigned int y, Uint32 pixel) {
    int bpp = surface_->surface()->format->BytesPerPixel;

    Uint8 *p = surface_->pixels() + y * surface_->surface()->pitch + x * bpp;

    set_pixel_(p, pixel);
  }

 private:
  Surface *surface_ = nullptr;

  Uint32 (*get_pixel_)(Uint8 *) = nullptr;
  void (*set_pixel_)(Uint8 *, Uint32) = nullptr;
};

}

#endif // SDLOBJ_SURFACEPAINTER_H_
