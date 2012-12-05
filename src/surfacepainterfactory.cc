#include "surfacepainterfactory.h"

using namespace sdlobj;

#ifdef COMPILE_TIME_BPP

SurfacePainterWrapper::SurfacePainterWrapper(Surface *surface) : painter(surface) {}

SurfacePainterWrapper::~SurfacePainterWrapper() = default;

void SurfacePainterWrapper::StartDrawing() {
  painter.StartDrawing();
}

void SurfacePainterWrapper::FinishDrawing() {
  painter.FinishDrawing();
}

SurfacePainterWrapper::Iterator SurfacePainterWrapper::Position(const unsigned int x, const unsigned int y) {
  return painter.Position(x, y);
}

SurfacePainterWrapper *GetSurfacePainter(Surface *surface) {
  return new SurfacePainterWrapper(surface);
}

#else

SurfacePainterWrapper *GetSurfacePainter(Surface *surface) {
  return SurfacePainterType::GetSurfacePainter(surface);
}

#endif
