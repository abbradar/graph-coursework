#include "common/math.h"
#include "common/debug.h"
#include "common/exception.h"
#include "rasterizer.h"

//#define WIREFRAME_MODEL

using namespace std;
using namespace sdlobj;

Rasterizer::Rasterizer() : scene_(nullptr), camera_(nullptr),
 surface_(nullptr), z_buffer_(0, 0), viewer_distance_(1), scale_(1) {
  set_scale(scale_);
}

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
    myfloat line_k = (value - a.x) / (b.x - a.x);
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
    myfloat line_k = (value - a.y) / (b.y - a.y);
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
    myfloat line_k = (value - a.z) / (b.z - a.z);
    result.x = line_k * (b.x - a.x) + a.x;
    result.y = line_k * (b.y - a.y) + a.y;
    result.z = value;
  }

  static bool Compare(const Point3D &point, const myfloat &value) {
    return Comparer::Compare(point.z, value);
  }
};

template<class Clipper> void Clip(IndexedTriangle &triangle, IndexedTriangle *triangles, size_t &triangles_num,
           Point3D *points, size_t &points_num, const myfloat &value) {
  int good_indexes[IndexedTriangle::kPointsSize] = { -1, -1, -1 };
  int bad_indexes[IndexedTriangle::kPointsSize] = { -1, -1, -1 };
  int good_i = 0, bad_i = 0;

  for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i)
    if (Clipper::Compare(points[triangle.points[i]], value))
      bad_indexes[bad_i++] = triangle.points[i];
    else
      good_indexes[good_i++] = triangle.points[i];

  switch (bad_i) {
    case 0:
      // let it be
      break;
    case 1:
      // split into two polygons
      Clipper::Intersect(points[good_indexes[1]], points[bad_indexes[0]], points[points_num], value);
      Clipper::Intersect(points[good_indexes[0]], points[bad_indexes[0]], points[bad_indexes[0]], value);
      triangle.Set(good_indexes[0], bad_indexes[0], good_indexes[1]);
      triangles[triangles_num].Set(bad_indexes[0], points_num, good_indexes[1]);
      ++triangles_num;
      ++points_num;
      break;
    case 2:
      // trim part
      Clipper::Intersect(points[good_indexes[0]], points[bad_indexes[0]], points[bad_indexes[0]], value);
      Clipper::Intersect(points[good_indexes[0]], points[bad_indexes[1]], points[bad_indexes[1]], value);
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

// Maximum number of splitted triangles after clipping
const int kClippedSize = 8;
// Number of clips
const int kClipNumber = 5;
const size_t kPolygonsSize = kClippedSize;
// One point for each clip can become unusable
const size_t POINTS_SIZE = kClippedSize * IndexedTriangle::kPointsSize + kClipNumber;

template<class T> bool ClipAll(IndexedTriangle *triangles, size_t &triangles_num,
                               Point3D *points, size_t &points_num, const myfloat &value) {
  size_t curr_triangles = triangles_num;
  for (size_t i = 0; i < curr_triangles; ++i)
    if (triangles[i].points[0] != -1)
      Clip<T>(triangles[i], triangles, triangles_num, points, points_num, value);
#if DEBUG_LEVEL == 4
  if (triangles_num > kPolygonsSize)
    throw Exception("There are more polygons than expected from clipping.");
  if (points_num > POINTS_SIZE)
    throw Exception("There are more points than expected from clipping.");
#endif
  return triangles_num == 1 && triangles[0].points[0] == -1;
}

void Rasterizer::DrawTriangle(const IndexedTriangle &source, const PointVector &points, const Color &color) {
  IndexedTriangle polygons[kPolygonsSize];
  Point3D points_buf[POINTS_SIZE];
  polygons[0] = IndexedTriangle(0, 1, 2);
  for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i)
    points_buf[i] = points[source.points[i]];
  size_t polygons_size = 1;
  size_t points_size = IndexedTriangle::kPointsSize;

  // clipping
  if (ClipAll<ZClipper<MyLessOrEqual<myfloat>>>(polygons, polygons_size, points_buf,
                                               points_size, 0)) return;
  if (ClipAll<XClipper<MyLessOrEqual<myfloat>>>(polygons, polygons_size, points_buf,
                                            points_size, 0)) return;
  if (ClipAll<XClipper<MyGreaterOrEqual<myfloat>>>(polygons, polygons_size, points_buf,
                                                   points_size, surface_->width() - 1)) return;
  if (ClipAll<YClipper<MyLessOrEqual<myfloat>>>(polygons, polygons_size, points_buf,
                                                points_size, 0)) return;
  if (ClipAll<YClipper<MyGreaterOrEqual<myfloat>>>(polygons,polygons_size, points_buf,
                                                   points_size, surface_->height() - 1)) return;

  // filling
  for (size_t i = 0; i < polygons_size; ++i) {
    if (polygons[i].points[0] != -1)
      FillTriangle(polygons[i], points_buf, color);
  }
}

void Rasterizer::FillTriangle(const IndexedTriangle &source, const Point3D *points, const Color &color) {
  // we'll just draw wire-frame model for now, without hidden line removal
  Uint32 pixel = surface_->ColorToPixel(color);
#ifdef WIREFRAME_MODEL
  surface_painter_.DrawLine(points[source.points[0]].x, points[source.points[0]].y,
      points[source.points[1]].x, points[source.points[1]].y, pixel);
  surface_painter_.DrawLine(points[source.points[1]].x, points[source.points[1]].y,
      points[source.points[2]].x, points[source.points[2]].y, pixel);
  surface_painter_.DrawLine(points[source.points[2]].x, points[source.points[2]].y,
      points[source.points[0]].x, points[source.points[0]].y, pixel);
#else
  ScreenLine3D lines[IndexedTriangle::kPointsSize];
  lines[0] = ScreenLine3D(&points[source.points[0]], &points[source.points[1]]);
  lines[1] = ScreenLine3D(&points[source.points[1]], &points[source.points[2]]);
  lines[2] = ScreenLine3D(&points[source.points[2]], &points[source.points[0]]);
  // sort them
  // this is bubble sort with cheating for kPointsSize = 3
  for (size_t i = 0; i < IndexedTriangle::kPointsSize - 1; ++i) {
    if (lines[i].y > lines[i + 1].y) swap(lines[i], lines[i + 1]);
  }
  for (size_t i = 0; i < IndexedTriangle::kPointsSize - 1; ++i) {
    if (lines[i].y == lines[i].fy) {
      swap(lines[i], lines[IndexedTriangle::kPointsSize - 1]);
      break;
    }
  }
  // fill lines
  ScreenLine3D *a = &lines[0], *b = &lines[1], *c = &lines[2];
  if (a->x != b->x) {
    if (a->x > b->x) swap(a, b);
#if DEBUG_LEVEL == 4
    FillLine(a, b, 0xFFFFFF);
    a->x += a->dx;
    a->z += a->dz;
    b->x += b->dx;
    b->z += b->dz;
    ++a->y;
#endif
    FillLines(a, b, pixel);
  } else {
    if (a->dx > b->dx) swap(a, b);
    FillLines(a, b, pixel);
    if (a->fy < b->fy) swap(a, b);
    if (a->x > c->x) swap(a, c);
    if (c->y != c->fy) {
      FillLines(a, c, pixel);
  #if DEBUG_LEVEL == 4
    } else {
      FillLine(a, b, 0xFFFFFF);
  #endif
    }
  }
#endif
}

void Rasterizer::FillLines(ScreenLine3D *a, ScreenLine3D *b, const Uint32 color) {
  ScreenLine3D *fa = a->fy < b->fy ? a : b;
  for (; a->y < fa->fy; ++a->y) {
    FillLine(a, b, color);

    // move line points
    a->x += a->dx;
    a->z += a->dz;
    b->x += b->dx;
    b->z += b->dz;
  }
  FillLine(a, b, color);
  b->y = a->y;
}

void Rasterizer::FillLine(const ScreenLine3D *a, const ScreenLine3D *b, const Uint32 color) {
  int ax = (int)a->x, bx = (int)b->x;
  myfloat z = a->z;
#if DEBUG_LEVEL == 4
  SetPixel(ax, a->y, z, 0xFFFFFF);
#endif
  if (ax < bx) {
    myfloat dz = (b->z - a->z) / (b->x - a->x);
#if DEBUG_LEVEL == 4
    z += dz;
    for (int x = ax + 1; x < bx; ++x)
#else
    for (int x = ax; x <= bx; ++x)
#endif
    {
      SetPixel(x, a->y, z, color);
      z += dz;
    }
#if DEBUG_LEVEL == 4
  SetPixel(bx, a->y, z, 0xFFFFFF);
#endif
  }
}

void Rasterizer::Render() {
  if (!(scene_ && camera_ && surface_)) throw Exception("Set scene, camera and surface");

  static const myfloat system_transform_m[] = { 0, -1, 0, 0, 0, 0, -1, 0, 1, 0, 0, 0, 0, 0, 0, 1};
  static const Matrix4 system_transform = Matrix4(system_transform_m);
  static const Point3D camera_direction = Point3D(1, 0, 0);

  Matrix4 transform = system_transform * camera_->GetMatrixTo();
  Point3D normal = camera_->GetRotateMatrixFrom() * camera_direction;

  z_buffer_.set_size(surface_->width(), surface_->height());
  z_buffer_.Clear();
  surface_painter_.StartDrawing();
  for (auto &object : scene_->objects()) {
    point_buffer_.clear();
    point_buffer_.reserve(object.positioned_points().size());
    for (const Point3D &p : object.positioned_points()) {
      Point3D dest = transform * p;
      myfloat k = viewer_distance_ / (viewer_distance_ + dest.z);
      dest.x = dest.x * k * scale_ + surface_->width() / 2.0;
      dest.y = dest.y * k * scale_ + surface_->height() / 2.0;
      point_buffer_.push_back(dest);
    }

    auto p = object.polygons().begin();
    auto n = object.positioned_polygon_normals().begin();
    for (auto pe = object.polygons().end(); p != pe; ++p, ++n) {
      if (Point3D::ScalarMul(normal, *n) <= 0) {
        DrawTriangle(*p, point_buffer_, object.color());
      }
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
  if (viewer_distance <= 0) throw Exception("Viewer distance should be > 0");
  viewer_distance_ = viewer_distance;
}

void Rasterizer::set_surface(Surface *surface) {
  surface_ = surface;
  surface_painter_.set_surface(surface_);
}

void Rasterizer::set_scale(const myfloat scale) {
  scale_ = scale;
}

Rasterizer::ScreenLine3D::ScreenLine3D() = default;

Rasterizer::ScreenLine3D::ScreenLine3D(const Point3D *a, const Point3D *b) : dx(0), dz(0) {
  if (a->y > b->y) swap(a, b);
  y = (int)a->y;
  fy = (int)b->y;
  x = a->x;
  if (y != fy) {
    myfloat dy = fy - y; /*b->y - a->y*/
    dx = (b->x - a->x) / dy;
    dz = (b->z - a->z) / dy;
    z = a->z;
  } else {
    dx = 0;
    dz = 0;
    z = min(a->z, b->z);
  }
}
