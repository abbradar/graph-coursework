#include <stdexcept>
#include "common/math.h"
#include "common/debug.h"
#include "rasterizer.h"

using namespace std;
using namespace sdlobj;

Rasterizer::Rasterizer() : scene_(nullptr), camera_(nullptr),
  surface_(nullptr), z_buffer_(0, 0), viewer_distance_(1) {}

/* These functions are safe to call with result = a | b */

template<class T> class MyLessOrEqual {
 public:
  static bool Compare(const T &a, const T &b) {
    return a <= b;
  }
};

template<class T> class MyGreaterOrEqual {
 public:
  static bool Compare(const T &a, const T &b) {
    return a >= b;
  }
};

template<class Comparer> class XClipper {
 public:
  static void Intersect(const Point3D &a, const Point3D &b, Point3D &result, const myfloat &value) {
    myfloat line_k = value - a.x / (b.z - a.x);
    result.y = line_k * (b.y - a.y) + a.y;
    result.z = line_k * (b.z - a.z) + a.z;
    result.x = value;
  }

  static bool Compare(const Point3D &point, const myfloat &value) {
    return Comparer::Compare(point.x, value);
  }
};

template<class Comparer> class YClipper {
 public:
  static void Intersect(const Point3D &a, const Point3D &b, Point3D &result, const myfloat &value) {
    myfloat line_k = value - a.y / (b.y - a.y);
    result.x = line_k * (b.x - a.x) + a.x;
    result.z = line_k * (b.z - a.z) + a.z;
    result.y = value;
  }

  static bool Compare(const Point3D &point, const myfloat &value) {
    return Comparer::Compare(point.y, value);
  }
};

template<class Comparer> class ZClipper {
 public:
  static void Intersect(const Point3D &a, const Point3D &b, Point3D &result, const myfloat &value) {
    myfloat line_k = value - a.z / (b.z - a.z);
    result.x = line_k * (b.x - a.x) + a.x;
    result.y = line_k * (b.y - a.y) + a.y;
    result.z = value;
  }

  static bool Compare(const Point3D &point, const myfloat &value) {
    return Comparer::Compare(point.z, value);
  }
};

template<class Clipper> void Clip(IndexedTriangle &triangle, IndexedTriangle *triangles, int &triangles_num,
           Point3D *points, int &points_num, const myfloat &value) {
  int good_indexes[IndexedTriangle::kPointsSize] = { -1, -1, -1 };
  int bad_indexes[IndexedTriangle::kPointsSize] = { -1, -1, -1 };
  int good_i = 0, bad_i = 0;

  for (int i = 0; i < IndexedTriangle::kPointsSize; ++i)
    if (Clipper::Compare(points[triangle.points[i]], value))
      bad_indexes[bad_i++] = triangle.points[i];
    else
      good_indexes[good_i++] = triangle.points[i];

  switch (bad_i) {
    case 0:
      // let it be
      break;
    case 1: {
      // split into two polygons
      Point3D good_points[2] = { points[good_indexes[0]], points[good_indexes[1]] };
      Clipper::Intersect(good_points[0], points[bad_indexes[0]], points[bad_indexes[0]], 0);
      Clipper::Intersect(good_points[1], points[bad_indexes[0]], points[points_num], 0);
      triangle.Set(good_indexes[0], bad_indexes[0], good_indexes[1]);
      triangles[triangles_num].Set(bad_indexes[0], points_num, good_indexes[1]);
      ++triangles_num;
      ++points_num;
      break;
    }
    case 2:
      // trim part
      Clipper::Intersect(points[good_indexes[0]], points[bad_indexes[0]], points[bad_indexes[0]], 0);
      Clipper::Intersect(points[good_indexes[0]], points[bad_indexes[1]], points[bad_indexes[1]], 0);
      break;
    case 3:
      // we flag triangle as "not in use" by setting it's first index to -1
      triangle.points[0] = -1;
      break;
    default:
      // something is horribly wrong here
      Assert(false);
      break;
  }
}

void Rasterizer::DrawTriangle(const IndexedTriangle &source, const Color &color) {
  // Maximum number of splitted triangles after clipping
  const int kClippedSize = 8;
  // Number of clips
  const int kClipNumber = 5;
  // Let's copy triangle to our local buffer for fast clipping
  IndexedTriangle polygons[kClippedSize];
  // One point for each clip can become unusable
  Point3D points[kClippedSize * IndexedTriangle::kPointsSize + kClipNumber];
  polygons[0] = IndexedTriangle(0, 1, 2);
  for (int i = 0; i < IndexedTriangle::kPointsSize; ++i)
    points[i] = point_buffer_[source.points[i]];
  int polygons_size = 1;
  int points_size = IndexedTriangle::kPointsSize;
  int curr_polygons = polygons_size;

  // clipping
  for (int i = 0; i < curr_polygons; ++i)
    Clip<ZClipper<MyLessOrEqual<myfloat>>>(polygons[i], polygons, polygons_size, points, points_size, 0);
  if (polygons_size == 1 && polygons[0].points[0] == -1) return;
  curr_polygons = polygons_size;
  /*for (int i = 0; i < curr_polygons; ++i)
    Clip<XClipper<MyLessOrEqual<myfloat>>>(polygons[i], polygons, polygons_size, points, points_size, 0);
  if (polygons_size == 1 && polygons[0].points[0] == -1) return;
  curr_polygons = polygons_size;
  for (int i = 0; i < curr_polygons; ++i)
    Clip<XClipper<MyGreaterOrEqual<myfloat>>>(polygons[i], polygons, polygons_size, points, points_size, surface_->width());
  if (polygons_size == 1 && polygons[0].points[0] == -1) return;
  curr_polygons = polygons_size;
  for (int i = 0; i < curr_polygons; ++i)
    Clip<YClipper<MyLessOrEqual<myfloat>>>(polygons[i], polygons, polygons_size, points, points_size, 0);
  if (polygons_size == 1 && polygons[0].points[0] == -1) return;
  curr_polygons = polygons_size;
  for (int i = 0; i < curr_polygons; ++i)
    Clip<YClipper<MyGreaterOrEqual<myfloat>>>(polygons[i], polygons, polygons_size, points, points_size, surface_->height());
  if (polygons_size == 1 && polygons[0].points[0] == -1) return;*/

  // filling
  for (int i = 0; i < polygons_size; ++i) {
    FillTriangle(polygons[i], points, color);
  }
}

// TODO: this is ugly thing to avoid clipping for now, remove later
void DrawLine(SurfacePainter &painter, int x1, int y1, int x2, int y2, Uint32 pixel) {
  int dx = x2 - x1, dy = y2 - y1;
  unsigned int x = x1, y = y1;
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

void Rasterizer::FillTriangle(const IndexedTriangle &source, const Point3D *points, const Color &color) {
  // we'll just draw wire-frame model for now, without hidden line removal
  Uint32 pixel = surface_->ColorToPixel(color);
  /*surface_painter_.DrawLine(points[source.points[0]].x, points[source.points[0]].y,
      points[source.points[1]].x, points[source.points[1]].y, pixel);
  surface_painter_.DrawLine(points[source.points[1]].x, points[source.points[1]].y,
      points[source.points[2]].x, points[source.points[2]].y, pixel);
  surface_painter_.DrawLine(points[source.points[2]].x, points[source.points[2]].y,
      points[source.points[0]].x, points[source.points[0]].y, pixel);*/
  DrawLine(surface_painter_, points[source.points[0]].x, points[source.points[0]].y,
        points[source.points[1]].x, points[source.points[1]].y, pixel);
  DrawLine(surface_painter_, points[source.points[1]].x, points[source.points[1]].y,
        points[source.points[2]].x, points[source.points[2]].y, pixel);
  DrawLine(surface_painter_, points[source.points[2]].x, points[source.points[2]].y,
        points[source.points[0]].x, points[source.points[0]].y, pixel);
}

void Rasterizer::Render() {
  const myfloat system_transform_m[] = { 0, -1, 0, 0, 0, 0, -1, 0, 1, 0, 0, 0, 0, 0, 0, 1};
  const Matrix4 system_transform = Matrix4(system_transform_m);

  if (!(scene_ && camera_ && surface_)) throw runtime_error("Set scene, camera and surface");

  Matrix4 transform = system_transform * camera_->GetMatrixTo();

  z_buffer_.set_size(surface_->width(), surface_->height());
  z_buffer_.clear();
  surface_painter_.StartDrawing();
  for (SceneObject &object : scene_->objects()) {
    point_buffer_.clear();
    point_buffer_.reserve(object.positioned_points().size());
    for (const Point3D &p : object.positioned_points()) {
      Point3D dest = transform * p;
      myfloat k = viewer_distance_ / (viewer_distance_ + dest.z);
      dest.x = dest.x * k + surface_->width() / 2.0;
      dest.y = dest.y * k + surface_->height() / 2.0;
      point_buffer_.push_back(dest);
    }

    for (const IndexedTriangle &index : object.polygons()) {
      DrawTriangle(index, object.color());
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

void Rasterizer::set_viewer_distance(myfloat viewer_distance) {
  if (viewer_distance <= 0) throw runtime_error("Viewer distance should be > 0");
  viewer_distance_ = viewer_distance;
}

void Rasterizer::set_surface(Surface *surface) {
  surface_ = surface;
  surface_painter_.set_surface(surface_);
}
