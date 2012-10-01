#include <stdexcept>
#include "common/math.h"
#include "rasterizer.h"

using namespace std;
using namespace sdlobj;

// TODO: fix values
const Point3D Rasterizer::kFocalPoint = Point3D(0, 0, 100);

Rasterizer::Rasterizer() : scene_(nullptr), camera_(nullptr),
  surface_(nullptr), z_buffer_size_(0) {}

void Rasterizer::Render() {
  if (!(scene_ && camera_ && surface_)) throw runtime_error("Set scene, camera and surface");

  ResetZBuffer();
  surface_painter_.StartDrawing();
  Matrix4 transform = Matrix4::Perspective(kFocalPoint.x, kFocalPoint.y, kFocalPoint.z);
  transform *= Matrix4::Translate(surface_->width() / 2.0, surface_->height() / 2.0, 0);
  transform *= camera_->GetMatrixTo();
  for (SceneObject &object : scene_->objects()) {
    object.ClearTransformed();
    object.ApplyTransform(transform);
    for (const IndexedPolygon &index : object.polygons()) {
      DrawTriangle(index, object.transformed_points(), object.color());
    }
  }
  surface_painter_.FinishDrawing();
}

void Rasterizer::set_scene(Scene *scene) {
  scene_ = scene;
}

void Rasterizer::set_camera(Position *camera) {
  camera_ = camera;
}

void Rasterizer::set_surface(Surface *surface) {
  surface_ = surface;
  surface_painter_.set_surface(surface_);
}

// TODO: this is ugly thing to avoid clipping for now, remove later
void DrawLine(SurfacePainter &painter, const Point3D &p1, const Point3D &p2, Uint32 pixel) {
  int dx = p2.x - p1.x, dy = p2.y - p1.y;
  unsigned int x = p1.x, y = p1.y;
  int sx = sign(dx), sy = sign(dy);
  dx = abs(dx), dy = abs(dy);
  bool swapped = dx < dy;
  if (swapped) swap(dx, dy);
  int e = 2 * dy - dx;
  int width = painter.surface()->width(), height = painter.surface()->height();
  for (int i = 0; i <= dx; ++i) {
    if (x >= 0 && y >= 0 && x < width && y < height)
      painter.SetPixel(x, y, pixel);
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


void Rasterizer::DrawTriangle(const IndexedPolygon &index, const SceneObject::PointVector &points, const Color &color) {
  // we'll just draw wire-frame model for now, without hidden line removal
  Uint32 pixel = surface_->ColorToPixel(color);
  DrawLine(surface_painter_, points[index.v1], points[index.v2], pixel);
  DrawLine(surface_painter_, points[index.v2], points[index.v3], pixel);
  DrawLine(surface_painter_, points[index.v3], points[index.v1], pixel);
}

void Rasterizer::ResetZBuffer() {
  // fairly fast for ~60 ops and adds safety
  int img_size = surface_->width() * surface_->height();
  if (z_buffer_size_ != img_size) {
    z_buffer_.reset(new my_float[img_size]);
    z_buffer_size_ = img_size;
  }
  float *z_buf = z_buffer_.get();
  for (int i = 0; i < img_size; ++i) {
    z_buf[i] = 0;
  }
}
