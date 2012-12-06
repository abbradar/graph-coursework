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
  Surface(Surface &&other);

  Surface &operator =(const Surface &other);

  ~Surface();

  inline const SDL_Surface *const surface() const {
    return surface_struct_;
  }

  SDL_Surface *surface();

  Surface DisplayConvert() const;
  Surface DisplayConvertAlpha() const;

  bool DisplayFormat() const;
  bool CompatibleFormat(const Surface &other) const;

  inline unsigned int width() const {
    return surface_struct_->w;
  }

  inline unsigned int height() const {
    return surface_struct_->h;
  }

  inline unsigned int bpp() const {
    return surface_struct_->format->BitsPerPixel;
  }

  void Lock();
  void Unlock();

  bool SetAlpha(const Uint32 flags, const Uint8 alpha);

  /** Sets transparent pixel for the surface
   * @param flags set SDL_SRCCOLORKEY to enable transparent pixel, SDL_RLEAXXEL for RLE acceleration
   */
  bool SetColorKey(const Uint32 flags, const Uint32 key);

  bool Blit(const Surface &other, const SDL_Rect &rect, const unsigned short x = 0, const unsigned short y = 0);
  bool Blit(const Surface &other, const unsigned short o_x, const unsigned short o_y, const unsigned short o_w,
            const unsigned short o_h, const unsigned short x = 0, const unsigned short y = 0);
  bool Blit(const Surface &other, const unsigned short x = 0, const unsigned short y = 0);

  bool FillRect(const SDL_Rect &dstrect, const Uint32 pixel);
  bool FillRect(const unsigned short x, const unsigned short y, const unsigned short w,
                const unsigned short h, const Uint32 pixel);
  bool Fill(const Uint32 pixel);

  Uint32 ColorToPixel(const sdlobj::Color &color) const;

  sdlobj::Color PixelToColor(const Uint32 pixel) const;

 private:
  struct SurfaceWrapper {
    SurfaceWrapper();
    SurfaceWrapper(SDL_Surface *surface_);
    ~SurfaceWrapper();

    SDL_Surface *surface;
  };

  friend class SDL;

  void set_surface(SDL_Surface *surface);
  void CopyOnWrite();

  std::shared_ptr<SurfaceWrapper> surface_;
  SDL_Surface *surface_struct_;
};

}

#include "sdl.h"

#endif // GRAPH_SDLOBJ_SURFACE_H_
