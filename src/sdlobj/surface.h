#ifndef GRAPH_SDLOBJ_SURFACE_H_
#define GRAPH_SDLOBJ_SURFACE_H_

#include <memory>
#include <SDL.h>
#include <SDL_video.h>

namespace sdlobj {

class SDL;

class Surface {
 public:
  Surface() : surface_(new SurfaceWrapper()) { }
  Surface(SDL_Surface *surface) : surface_(new SurfaceWrapper(surface)) { }

  /** Create surface with same parameters as screen surface */
  Surface(int width, int height);
  Surface(int width, int height, Uint32 flags, int bpp, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
  Surface(const Surface &other) : surface_(other.surface_) {}

  inline const SDL_Surface *surface() {
    return surface_->surface;
  }

  inline unsigned int width() {
    return surface_->surface->w;
  }

  inline unsigned int height() {
    return surface_->surface->h;
  }

  inline unsigned int bpp() {
    return surface_->surface->format->BitsPerPixel;
  }

  inline Uint8 *pixels() {
    return (Uint8 *)surface_->surface->pixels;
  }

  inline void Lock() {
    SDL_LockSurface(surface_->surface);
  }

  inline void Unlock() {
    SDL_UnlockSurface(surface_->surface);
  }

  void SetAlpha(Uint32 flags, Uint8 alpha);

  /** Sets transparent pixel for the surface
   * @param flags set SDL_SRCCOLORKEY to enable transparent pixel, SDL_RLEAXXEL for RLE acceleration
   */
  void SetColorKey(Uint32 flags, Uint32 key);

  void Blit(const Surface &other, SDL_Rect &rect, short x = 0, short y = 0);
  void Blit(const Surface &other, short x = 0, short y = 0);

  void FillRect(SDL_Rect &dstrect, Uint32 pixel);
  void Fill(Uint32 pixel);

  Uint32 ColorToPixel(const SDL_Color &color);

  SDL_Color PixelToColor(const Uint32 &pixel);

 private:
  struct SurfaceWrapper {
    SurfaceWrapper() {}
    SurfaceWrapper(SDL_Surface * const &surface_) : surface(surface_) {}
    ~SurfaceWrapper();

    SDL_Surface *surface = nullptr;
  };

  friend class SDL;

  std::shared_ptr<SurfaceWrapper> surface_;
};

}

#include "sdl.h"

#endif // GRAPH_SDLOBJ_SURFACE_H_
