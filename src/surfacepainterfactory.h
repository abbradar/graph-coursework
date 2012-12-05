#ifndef GRAPH_SURFACEPAINTERFACTORY_H_
#define GRAPH_SURFACEPAINTERFACTORY_H_

#include "sdlobj/surfacepainter.h"
#include "config.h"

#ifdef COMPILE_TIME_BPP

template <unsigned kBpp> class GetPainterType {};

template <> class GetPainterType<8> {
 public:
  typedef sdlobj::PixelSurfacePainter<sdlobj::DrawerPixel1> PainterType;
};

template <> class GetPainterType<16> {
 public:
  typedef sdlobj::PixelSurfacePainter<sdlobj::DrawerPixel2> PainterType;
};

template <> class GetPainterType<24> {
 public:
  typedef sdlobj::PixelSurfacePainter<sdlobj::DrawerPixel3> PainterType;
};

template <> class GetPainterType<32> {
 public:
  typedef sdlobj::PixelSurfacePainter<sdlobj::DrawerPixel4> PainterType;
};

class SurfacePainterWrapper {
 public:
  typedef typename GetPainterType<kDefaultBpp>::PainterType PainterType;
  typedef PainterType::Iterator Iterator;

  SurfacePainterWrapper(sdlobj::Surface *surface);
  ~SurfacePainterWrapper();

  void StartDrawing();
  void FinishDrawing();

  inline bool drawing() const {
    return painter.drawing();
  }

  inline sdlobj::Surface *surface() {
    return painter.surface();
  }

  inline const sdlobj::Surface *surface() const {
    return painter.surface();
  }

  Iterator Position(const unsigned int x, const unsigned int y);

  inline Uint32 GetPixel(const unsigned int x, const unsigned int y) const {
    return painter.GetPixel(x, y);
  }

  inline void SetPixel(const unsigned int x, const unsigned int y, const Uint32 pixel) {
    painter.SetPixel(x, y, pixel);
  }

 private:
  PainterType painter;
};

#else

typedef sdlobj::SurfacePainterType SurfacePainterWrapper;

#endif

SurfacePainterWrapper *GetSurfacePainter(sdlobj::Surface *surface);

#endif // SURFACEPAINTERFACTORY_H
