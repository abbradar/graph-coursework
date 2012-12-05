#include "common/exception.h"
#include "surfacepainter.h"

using namespace std;

namespace sdlobj {

SurfacePainter::Iterator::Iterator() = default;
SurfacePainter::Iterator::~Iterator() = default;

SurfacePainter::~SurfacePainter() {
  if (drawing_) FinishDrawing();
}

void SurfacePainter::StartDrawing()  {
  if (drawing_) throw Exception("Drawing has already started.");
  surface_->Lock();
  drawing_ = true;
}

void SurfacePainter::FinishDrawing() {
  if (!drawing_) throw Exception("Drawing has not started.");
  surface_->Unlock();
  drawing_ = false;
}

SurfacePainter *SurfacePainter::GetSurfacePainter(Surface *surface) {
  switch (surface->bpp()) {
    case 8: return new PixelSurfacePainter<DrawerPixel1>(surface);
    case 16: return new PixelSurfacePainter<DrawerPixel2>(surface);
    case 24: return new PixelSurfacePainter<DrawerPixel3>(surface);
    case 32: return new PixelSurfacePainter<DrawerPixel4>(surface);
    default: throw Exception("Invalid bpp.");
  }
}

SurfacePainter::SurfacePainter(Surface *surface) : surface_(surface), surface_struct_(surface_->surface()),
  drawing_(false) {}

}
