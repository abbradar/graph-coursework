#include <algorithm>
#include "common/math.h"
#include "common/debug.h"
#include "surfacepainter.h"

using namespace std;

namespace sdlobj {

SurfacePainter::SurfacePainter() : surface_(nullptr), surface_struct_(nullptr),
 drawing_(false), get_pixel_(nullptr), set_pixel_(nullptr) {}

SurfacePainter::~SurfacePainter() {
  if (drawing_) FinishDrawing();
}

void SetPixel1(Uint8 *p, const Uint32 pixel) {
  *p = pixel;
}

void SetPixel2(Uint8 *p, const Uint32 pixel) {
  *(Uint16 *)p = pixel;
}

void SetPixel3(Uint8 *p, const Uint32 pixel) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    p[0] = (pixel >> 16) & 0xFF;
    p[1] = (pixel >> 8) & 0xFF;
    p[2] = pixel & 0xFF;
#else
    p[0] = pixel & 0xFF;
    p[1] = (pixel >> 8) & 0xFF;
    p[2] = (pixel >> 16) & 0xFF;
#endif
}

void SetPixel4(Uint8 *p, const Uint32 pixel) {
  *(Uint32 *)p = pixel;
}

Uint32 GetPixel1(const Uint8 *p) {
  return *p;
}

Uint32 GetPixel2(const Uint8 *p) {
  return *(Uint16 *)p;
}

Uint32 GetPixel3(const Uint8 *p) {
  if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    return p[0] << 16 | p[1] << 8 | p[2];
  else
    return p[0] | p[1] << 8 | p[2] << 16;
}

Uint32 GetPixel4(const Uint8 *p) {
  return *(Uint32 *)p;
}

SurfacePainter::SurfacePainter(Surface *surface) : SurfacePainter() {
  set_surface(surface);
}

void SurfacePainter::set_surface(Surface *surface) {
  surface_ = surface;
  surface_struct_ = surface->surface();
  switch (surface_struct_->format->BytesPerPixel) {
   case 1:
    get_pixel_ = &GetPixel1;
    set_pixel_ = &SetPixel1;
    break;
   case 2:
    get_pixel_ = &GetPixel2;
    set_pixel_ = &SetPixel2;
    break;
   case 3:
    get_pixel_ = &GetPixel3;
    set_pixel_ = &SetPixel3;
    break;
   case 4:
    get_pixel_ = &GetPixel4;
    set_pixel_ = &SetPixel4;
    break;
   default:
    AssertMsg(false, "Unknown bpp value");
  }
}

void SurfacePainter::DrawLine(const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2, const Uint32 pixel) {
  int dx = x2 - x1, dy = y2 - y1;
  unsigned int x = x1, y = y1;
  int sx = Sign(dx), sy = Sign(dy);
  dx = abs(dx), dy = abs(dy);
  bool swapped = dx < dy;
  if (swapped) swap(dx, dy);
  int e = 2 * dy - dx;
  for (int i = 0; i <= dx; ++i) {
    SetPixel(x, y, pixel);
    if (e >= 0) {
      if (swapped)
        x = x + sx;
      else
        y = y + sy;
      e -= 2 * dx;
    }
    if (swapped)
      y = y + sy;
    else
      x = x + sx;
    e += 2 * dy;
  }
}

}
