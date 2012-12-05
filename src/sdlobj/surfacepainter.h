#ifndef SDLOBJ_SURFACEPAINTER_H_
#define SDLOBJ_SURFACEPAINTER_H_

#include <algorithm>
#include "common/math.h"
#include "common/debug.h"
#include "surface.h"

#if DEBUG_LEVEL >= 4
#include <boost/format.hpp>
#include "common/exception.h"
#endif

namespace sdlobj {

struct DrawerPixel1 {
  typedef Uint8 *PointerType;

  static inline Uint32 GetPixel(const PointerType p) {
    return *p;
  }

  static inline void SetPixel(PointerType p, const Uint32 pixel) {
    *p = pixel;
  }

  static inline PointerType Position(SDL_Surface *surface, const unsigned x, const unsigned y) {
    return (PointerType)surface->pixels + y * surface->w + x;
  }

  inline static void Move(PointerType &p, int value) {
    p += value;
  }

  inline static void Increment(PointerType &p) {
    ++p;
  }

  inline static void Decrement(PointerType &p) {
    --p;
  }
};

struct DrawerPixel2 {
  typedef Uint16 *PointerType;

  static inline Uint32 GetPixel(const PointerType p) {
    return *p;
  }

  static inline void SetPixel(PointerType p, const Uint32 pixel) {
    *p = pixel;
  }

  static inline PointerType Position(SDL_Surface *surface, const unsigned x, const unsigned y) {
    return (PointerType)surface->pixels + y * surface->w + x;
  }

  inline static void Move(PointerType &p, int value) {
    p += value;
  }

  inline static void Increment(PointerType &p) {
    ++p;
  }

  inline static void Decrement(PointerType &p) {
    --p;
  }
};

struct DrawerPixel3 {
  typedef Uint8 *PointerType;

  static inline Uint32 GetPixel(const PointerType p) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    return p[0] << 16 | p[1] << 8 | p[2];
#else
    return p[0] | p[1] << 8 | p[2] << 16;
#endif
  }

  static inline void SetPixel(PointerType p, const Uint32 pixel) {
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

  static inline PointerType Position(SDL_Surface *surface, const unsigned x, const unsigned y) {
    return (PointerType)surface->pixels + y * surface->w + x;
  }

  inline static void Move(PointerType &p, int value) {
    p += value * 3;
  }

  inline static void Increment(PointerType &p) {
    p += 3;
  }

  inline static void Decrement(PointerType &p) {
    p -= 3;
  }
};

struct DrawerPixel4 {
  typedef Uint32 *PointerType;

  static inline Uint32 GetPixel(const PointerType p) {
    return *p;
  }

  static inline void SetPixel(PointerType p, const Uint32 pixel) {
    *p = pixel;
  }

  static inline PointerType Position(SDL_Surface *surface, const unsigned x, const unsigned y) {
    return (PointerType)surface->pixels + y * surface->w + x;
  }

  inline static void Move(PointerType &p, int value) {
    p += value;
  }

  inline static void Increment(PointerType &p) {
    ++p;
  }

  inline static void Decrement(PointerType &p) {
    --p;
  }
};

class SurfacePainter {
 public:
  class Iterator {
   public:
    Iterator();
    virtual ~Iterator();

    inline virtual Uint32 GetPixel() = 0;
    inline virtual void SetPixel(const Uint32 pixel) = 0;

    inline virtual Iterator &operator ++() = 0;
    inline virtual Iterator &operator --() = 0;
  };

  static SurfacePainter *GetSurfacePainter(Surface *surface);

  SurfacePainter(Surface *surface);
  virtual ~SurfacePainter();

  void StartDrawing();
  void FinishDrawing();

  inline bool drawing() const {
    return drawing_;
  }

  inline Surface *surface() {
    return surface_;
  }

  inline const Surface *surface() const {
    return surface_;
  }

  /** Get the pixel at (x, y) */
  virtual Uint32 GetPixel(const unsigned int x, const unsigned int y) const = 0;

  /** Set the pixel at (x, y) to the given value */
  virtual void SetPixel(const unsigned int x, const unsigned int y, const Uint32 pixel) = 0;

  virtual Iterator *VirtualPosition(const unsigned int x, const unsigned int y) = 0;

 protected:
  inline SDL_Surface *surface_struct() const {
    return surface_struct_;
  }

 private:
  Surface *surface_;
  SDL_Surface *surface_struct_;
  bool drawing_;
};

template <class Pixel> class PixelSurfacePainter : public SurfacePainter {
 public:
  typedef typename Pixel::PointerType PointerType;

  class Iterator : public SurfacePainter::Iterator {
   public:
    Iterator() = default;

    inline Uint32 GetPixel() {
      return Pixel::GetPixel(ptr_);
    }

    inline void SetPixel(const Uint32 pixel) {
      Pixel::SetPixel(ptr_, pixel);
    }

    inline Iterator &operator ++() {
      Pixel::Increment(ptr_);
      return *this;
    }

    inline Iterator &operator --() {
      Pixel::Decrement(ptr_);
      return *this;
    }

   private:
    friend class PixelSurfacePainter;

    Iterator(PointerType ptr) : ptr_(ptr) {}

    PointerType ptr_;
  };

  PixelSurfacePainter(Surface *surface) : SurfacePainter(surface) {}

  Iterator Position(const unsigned int x, const unsigned int y) {
#if DEBUG_LEVEL >= 4
    if (x >= (unsigned int)surface_struct_->w || y >= (unsigned int)surface_struct_->h) {
      AssertMsg(false, (boost::format("Coordinates out of bounds: %1%,%2%") % x % y).str().data());
    }
#endif
    return Iterator(Pixel::Position(surface_struct(), x, y));
  }

  virtual Iterator *VirtualPosition(const unsigned int x, const unsigned int y) {
#if DEBUG_LEVEL >= 4
    if (x >= (unsigned int)surface_struct_->w || y >= (unsigned int)surface_struct_->h) {
      AssertMsg(false, (boost::format("Coordinates out of bounds: %1%,%2%") % x % y).str().data());
    }
#endif
    return new Iterator(Pixel::Position(surface_struct(), x, y));
  }

  inline virtual Uint32 GetPixel(const unsigned int x, const unsigned int y) const {
#if DEBUG_LEVEL >= 4
    if (x >= (unsigned int)surface_struct_->w || y >= (unsigned int)surface_struct_->h) {
      AssertMsg(false, (boost::format("Coordinates out of bounds: %1%,%2%") % x % y).str().data());
    }
#endif
    return Pixel::GetPixel(Pixel::Position(surface_struct(), x, y));
  }

  inline void SetPixel(const unsigned int x, const unsigned int y, const Uint32 pixel) {
#if DEBUG_LEVEL >= 4
    if (x >= (unsigned int)surface_struct_->w || y >= (unsigned int)surface_struct_->h) {
      AssertMsg(false. (boost::format("Coordinates out of bounds: %1%, %2%") % x % y).str().data());
    }
#endif
    Pixel::SetPixel(Pixel::Position(surface_struct(), x, y), pixel);
  }
};

/** Draws line (integer Bresenham's algorithm) */
template <class Painter> void DrawLine(Painter *painter, const unsigned int x1, const unsigned int y1, const unsigned int x2, const unsigned int y2,
              const Uint32 pixel) {
  int dx = x2 - x1, dy = y2 - y1;
  unsigned int x = x1, y = y1;
  int sx = Sign(dx), sy = Sign(dy);
  dx = abs(dx), dy = abs(dy);
  bool swapped = dx < dy;
  if (swapped) std::swap(dx, dy);
  int e = 2 * dy - dx;
  for (int i = 0; i <= dx; ++i) {
    painter->SetPixel(x, y, pixel);
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

#endif // SDLOBJ_SURFACEPAINTER_H_
