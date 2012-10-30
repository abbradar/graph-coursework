#include "surface.h"

namespace sdlobj {

Surface::Surface() : surface_(), surface_struct_(nullptr) {}
Surface::Surface(SDL_Surface *surface) : surface_(new SurfaceWrapper(surface)), surface_struct_(surface_->surface) {}
Surface::Surface(const Surface &other) : surface_(other.surface_), surface_struct_(nullptr) {
  if (surface_) {
    surface_struct_ = surface_->surface;
  }
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

Uint32 Surface::ColorToPixel(const Color &color) {
  return SDL_MapRGB(surface_struct_->format, color.r, color.g, color.b);
}

Color Surface::PixelToColor(const Uint32 &pixel) {
  Color a;
  SDL_GetRGB(pixel, surface_struct_->format, &a.r, &a.g, &a.b);
  return a;
}

void Surface::set_surface(SDL_Surface *surface) {
  surface_->surface = surface;
  surface_struct_ = surface;
}

void Surface::Blit(const Surface &other, SDL_Rect &rect, short x, short y) {
  SDL_Rect a = { .x = x, .y = y };
  SDL_BlitSurface(other.surface_struct_, &rect, surface_struct_, &a);
}

void Surface::Blit(const Surface &other, short x, short y) {
  SDL_Rect a = { .x = x, .y = y };
  SDL_BlitSurface(other.surface_struct_, nullptr, surface_struct_, &a);
}

void Surface::FillRect(SDL_Rect &dstrect, Uint32 pixel) {
  SDL_FillRect(surface_struct_, &dstrect, pixel);
}

void Surface::Fill(Uint32 pixel) {
  SDL_FillRect(surface_struct_, nullptr, pixel);
}

void Surface::SetAlpha(Uint32 flags, Uint8 alpha) {
  SDL_SetAlpha(surface_struct_, flags, alpha);
}

void Surface::SetColorKey(Uint32 flags, Uint32 key) {
  SDL_SetColorKey(surface_struct_, flags, key);
}

Surface::SurfaceWrapper::SurfaceWrapper() : surface(nullptr) {}
Surface::SurfaceWrapper::SurfaceWrapper(SDL_Surface *surface_) : surface(surface_) {}

Surface::SurfaceWrapper::~SurfaceWrapper() {
  if (surface) SDL_FreeSurface(surface);
}

}
