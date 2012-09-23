#include "surface.h"

using namespace sdlobj;

Surface::Surface(int width, int height, Uint32 flags, int bpp, Uint32 Rmask, Uint32 Gmask,
                 Uint32 Bmask, Uint32 Amask) : Surface() {
  surface_->surface = SDL_CreateRGBSurface(flags, width, height, bpp, Rmask, Gmask, Bmask, Amask);
}

Surface::Surface(int width, int height) : Surface() {
  const SDL_Surface *screen = SDL::instance().surface().surface();

  surface_->surface = SDL_CreateRGBSurface(screen->flags, width, height, screen->format->BitsPerPixel,
                                  screen->format->Rmask, screen->format->Gmask, screen->format->Bmask,
                                  screen->format->Amask);
}

Uint32 Surface::ColorToPixel(const SDL_Color &color) {
  return SDL_MapRGB(surface()->format, color.r, color.g, color.b);
}

SDL_Color Surface::PixelToColor(const Uint32 &pixel) {
  SDL_Color a;
  SDL_GetRGB(pixel, surface()->format, &a.r, &a.g, &a.b);
  return a;
}

void Surface::Blit(const Surface &other, SDL_Rect &rect, short x, short y) {
  SDL_Rect a = { .x = x, .y = y };
  SDL_BlitSurface(other.surface_->surface, &rect, surface_->surface, &a);
}

void Surface::Blit(const Surface &other, short x, short y) {
  SDL_Rect a = { .x = x, .y = y };
  SDL_BlitSurface(other.surface_->surface, nullptr, surface_->surface, &a);
}

void Surface::FillRect(SDL_Rect &dstrect, Uint32 pixel) {
  SDL_FillRect(surface_->surface, &dstrect, pixel);
}

void Surface::Fill(Uint32 pixel) {
  SDL_FillRect(surface_->surface, nullptr, pixel);
}

void Surface::SetAlpha(Uint32 flags, Uint8 alpha) {
  SDL_SetAlpha(surface_->surface, flags, alpha);
}

void Surface::SetColorKey(Uint32 flags, Uint32 key) {
  SDL_SetColorKey(surface_->surface, flags, key);
}

Surface::SurfaceWrapper::~SurfaceWrapper() {
  SDL_FreeSurface(surface);
}
