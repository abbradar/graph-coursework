#include "surface.h"

namespace sdlobj {

Surface::Surface() : surface_(), surface_struct_(nullptr) {}
Surface::Surface(SDL_Surface *surface) : surface_(new SurfaceWrapper(surface)), surface_struct_(surface_->surface) {}
Surface::Surface(const Surface &other) : surface_(other.surface_), surface_struct_(other.surface_struct_) {}

Surface &Surface::operator =(const Surface &other) {
  surface_ = other.surface_;
  surface_struct_ = other.surface_struct_;
  return *this;
}

Surface::Surface(const unsigned int width, const unsigned int height, const Uint32 flags, const unsigned int bpp,
                 const Uint32 Rmask, const Uint32 Gmask, const Uint32 Bmask, const Uint32 Amask)
 : surface_(new SurfaceWrapper()) {
  surface_->surface = SDL_CreateRGBSurface(flags, width, height, bpp, Rmask, Gmask, Bmask, Amask);
  surface_struct_ = surface_->surface;
}

Surface::Surface(const unsigned int width, const unsigned int height) : surface_(new SurfaceWrapper()) {
  const SDL_Surface *screen = SDL::instance().surface().surface();
  Uint32 flags = screen->flags & (SDL_SWSURFACE | SDL_HWSURFACE | SDL_SRCCOLORKEY | SDL_SRCALPHA);

  surface_->surface = SDL_CreateRGBSurface(flags, width, height, screen->format->BitsPerPixel,
                                  screen->format->Rmask, screen->format->Gmask, screen->format->Bmask,
                                  screen->format->Amask);
  surface_struct_ = surface_->surface;
}

Surface::~Surface() {
  if (!SDL::instance().initialized() && surface_) {
    surface_->surface = nullptr;
  }
}

SDL_Surface *Surface::surface() {
  CopyOnWrite();
  return surface_struct_;
}

Uint32 Surface::ColorToPixel(const Color &color) {
  return SDL_MapRGB(surface_struct_->format, color.r, color.g, color.b);
}

Color Surface::PixelToColor(const Uint32 pixel) {
  Color a;
  SDL_GetRGB(pixel, surface_struct_->format, &a.r, &a.g, &a.b);
  return a;
}

void Surface::set_surface(SDL_Surface *surface) {
  surface_->surface = surface;
  surface_struct_ = surface;
}

void Surface::CopyOnWrite() {
  if (!surface_.unique()) {
    set_surface(SDL_ConvertSurface(surface_struct_,surface_struct_->format,
                                   surface_struct_->flags));
  }
}

bool Surface::Blit(const Surface &other, const SDL_Rect &rect, const unsigned short x, const unsigned short y) {
  SDL_Rect a = { .x = (short)x, .y = (short)y };
  return SDL_BlitSurface(other.surface_struct_, (SDL_Rect *)&rect, surface_struct_, &a) == 0;
}

bool Surface::Blit(const Surface &other, const unsigned short o_x, const unsigned short o_y,
                   const unsigned short o_w, const unsigned short o_h, const unsigned short x, const unsigned short y) {
  SDL_Rect a = { .x = (short)o_x, .y = (short)o_y, .w = o_w, .h = o_h };
  return Blit(other, a, x, y);
}

bool Surface::Blit(const Surface &other, const unsigned short x, const unsigned short y) {
  SDL_Rect a = { .x = (short)x, .y = (short)y };
  return SDL_BlitSurface(other.surface_struct_, nullptr, surface_struct_, &a) == 0;
}

bool Surface::FillRect(const SDL_Rect &dstrect, const Uint32 pixel) {
  return SDL_FillRect(surface_struct_, (SDL_Rect *)&dstrect, pixel) == 0;
}

bool Surface::FillRect(const unsigned short x, const unsigned short y, const unsigned short w,
                       const unsigned short h, const Uint32 pixel) {
  SDL_Rect a = { .x = (short)x, .y = (short)y, .w = w, .h = h };
  return FillRect(a, pixel);
}

bool Surface::Fill(const Uint32 pixel) {
  return SDL_FillRect(surface_struct_, nullptr, pixel) == 0;
}

bool Surface::SetAlpha(const Uint32 flags, const Uint8 alpha) {
  return SDL_SetAlpha(surface_struct_, flags, alpha) == 0;
}

bool Surface::SetColorKey(const Uint32 flags, const Uint32 key) {
  return SDL_SetColorKey(surface_struct_, flags, key) == 0;
}

Surface::SurfaceWrapper::SurfaceWrapper() : surface(nullptr) {}
Surface::SurfaceWrapper::SurfaceWrapper(SDL_Surface *surface_) : surface(surface_) {}

Surface::SurfaceWrapper::~SurfaceWrapper() {
  if (surface) SDL_FreeSurface(surface);
}

}
