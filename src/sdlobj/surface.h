#ifndef GRAPH_SDLOBJ_SURFACE_H_
#define GRAPH_SDLOBJ_SURFACE_H_

#include <memory>
#include <SDL.h>
#include <SDL_video.h>
#include "color.h"

namespace sdlobj {

class SDL;

class Surface {
public:
  Surface();

  Surface(SDL_Surface *surface);

  /** Create surface with same parameters as screen surface */
  Surface(const unsigned int width, const unsigned int height);
  Surface(const unsigned int width, const unsigned int height, const Uint32 flags, const unsigned int bpp,
          const Uint32 Rmask, const Uint32 Gmask, const Uint32 Bmask, const Uint32 Amask);
  Surface(const Surface &other);

  ~Surface();

  inline const SDL_Surface *const surface() {
    return surface_struct_;
  }

  inline unsigned int width() {
    return surface_struct_->w;
  }

  inline unsigned int height() {
    return surface_struct_->h;
  }

  inline unsigned int bpp() {
    return surface_struct_->format->BitsPerPixel;
  }

  inline void Lock() {
    SDL_LockSurface(surface_struct_);
  }

  inline void Unlock() {
    SDL_UnlockSurface(surface_struct_);
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

  Uint32 ColorToPixel(const sdlobj::Color &color);

  sdlobj::Color PixelToColor(const Uint32 &pixel);

 private:
  struct SurfaceWrapper {
    SurfaceWrapper();
    SurfaceWrapper(SDL_Surface *surface_);
    ~SurfaceWrapper();

    SDL_Surface *surface;
  };

  friend class SDL;

  void set_surface(SDL_Surface *surface);

  std::shared_ptr<SurfaceWrapper> surface_;
  SDL_Surface *surface_struct_;
};

}

#include "sdl.h"

#endif // GRAPH_SDLOBJ_SURFACE_H_
