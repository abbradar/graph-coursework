#include "common/math.h"
#include "common/debug.h"
#include "common/exception.h"
#include "rasterizer.h"

//#define WIREFRAME_MODEL
//#define NO_NORMAL_FACE_CLIPPING

using namespace std;
using namespace sdlobj;

Rasterizer::Rasterizer() : scene_(nullptr), camera_(nullptr),
 surface_(nullptr), trace_(false), z_buffer_(0, 0), viewer_distance_(1),
 scale_(1) {
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

void Rasterizer::DrawTriangle(const IndexedTriangle &source, const Point3DVector &points,
                              const Model &model, const Material &material) {
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
      FillTriangle(polygons[i], points_buf, material);
  }
}

void Rasterizer::FillTriangle(const IndexedTriangle &source, const Point3D *points, const Material &material) {
  static_assert(IndexedTriangle::kPointsSize == 3, "Polygons (number of points != 3) is not supported");
  Uint32 pixel = surface_->ColorToPixel(material.color());
#ifdef WIREFRAME_MODEL
  surface_painter_.DrawLine(points[source.points[0]].x, points[source.points[0]].y,
      points[source.points[1]].x, points[source.points[1]].y, pixel);
  surface_painter_.DrawLine(points[source.points[1]].x, points[source.points[1]].y,
      points[source.points[2]].x, points[source.points[2]].y, pixel);
  surface_painter_.DrawLine(points[source.points[2]].x, points[source.points[2]].y,
      points[source.points[0]].x, points[source.points[0]].y, pixel);
#else
  const Point3D *points_ptr[IndexedTriangle::kPointsSize];
  for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
    points_ptr[i] = &points[source.points[i]];
  }
  // sort them
  for (size_t i = IndexedTriangle::kPointsSize - 1; i > 0; --i) {
    for (size_t j = 0; j < i; ++j) {
      if (points_ptr[j]->y > points_ptr[j + 1]->y) swap(points_ptr[j], points_ptr[j + 1]);
    }
  }

  if ((unsigned)points_ptr[0]->y == (unsigned)points_ptr[1]->y) {
    if (points_ptr[0]->x > points_ptr[1]->x) swap(points_ptr[0], points_ptr[1]);
    ScreenLine3D a(*points_ptr[0], *points_ptr[2]);
    ScreenLine3D b(*points_ptr[1], *points_ptr[2]);
#if DEBUG_LEVEL == 4
    FillLine(a.y, a.x, b.x, a.z, b.z, 0xFFFFFF);
    a.Advance();
    b.Advance();
#endif
    FillLines(a, b, pixel, a.fy);
  } else {
    ScreenLine3D big(*points_ptr[0], *points_ptr[2]);
    ScreenLine3D small1(*points_ptr[0], *points_ptr[1]);
    if (big.dx <= small1.dx) {
      FillLines(big, small1, pixel, small1.fy);

      if (big.y != big.fy) {
        ScreenLine3D small2(*points_ptr[1], *points_ptr[2]);
        FillLines(big, small2, pixel, big.fy);
  #if DEBUG_LEVEL == 4
      } else {
        FillLine(big.y, big.x, small1.x, big.z, small1.z, 0xFFFFFF);
  #endif
      }
    } else {
      FillLines(small1, big, pixel, small1.fy);

      if (big.y != big.fy) {
        ScreenLine3D small2(*points_ptr[1], *points_ptr[2]);
        FillLines(small2, big, pixel, big.fy);
  #if DEBUG_LEVEL == 4
      } else {
        FillLine(big.y, small1.x, big.x, small1.z, big.z, 0xFFFFFF);
  #endif
      }
    }
  }
#endif
}

void Rasterizer::FillLines(ScreenLine3D &a, ScreenLine3D &b, const Uint32 color,
                           const unsigned int fy) {
  unsigned int y = a.y;
  for (; y < fy; ++y) {
    FillLine(y, a.x, b.x, a.z, b.z, color);

    // move line points
    a.Advance();
    b.Advance();
  }
  FillLine(y, a.x, b.x, a.z, b.z, color);
}

void Rasterizer::FillLine(const unsigned int y, const unsigned int ax, const unsigned int bx,
                          const myfloat az, const myfloat bz, const Uint32 color) {
  myfloat z = az;
#if DEBUG_LEVEL == 4
  SetPixel(ax, y, z, 0xFFFFFF);
#endif
  if (ax < bx) {
    myfloat dz = (bz - az) / (bx - ax);
#if DEBUG_LEVEL == 4
    z += dz;
    for (unsigned int x = ax + 1; x < bx; ++x)
#else
    for (unsigned int x = ax; x <= bx; ++x)
#endif
    {
      SetPixel(x, y, z, color);
      z += dz;
    }
#if DEBUG_LEVEL == 4
  SetPixel(bx, y, z, 0xFFFFFF);
#endif
  }
}

void Rasterizer::FirstPass() {
  if (!(scene_ && camera_ && surface_)) throw Exception("Set scene, camera and surface");

  static const myfloat system_transform_m[] = {0, -1, 0, 0,
                                               0, 0, -1, 0,
                                               1, 0, 0, 0,
                                               0, 0, 0, 1};
#ifndef NO_NORMAL_FACE_CLIPPING
  static const Point3D camera_direction = Point3D(1, 0, 0);
#endif

  Matrix4 transform(system_transform_m);
  transform *= camera_->GetMatrixTo();
#ifndef NO_NORMAL_FACE_CLIPPING
  Point3D normal = camera_->GetRotateMatrixFrom() * camera_direction;
#endif

  CacheMap new_cache;

  for (auto &object : scene_->objects()) {
    SceneObject *object_ptr = object.get();
    CacheMap::iterator hit = new_cache.find(object_ptr);
    if (hit == new_cache.end()) {
      CacheMap::iterator old_hit = cache_.find(object_ptr);
      if (old_hit != cache_.end()) {
        std::unique_ptr<ObjectCache> ptr;
        ptr.swap(old_hit->second);
        hit = new_cache.insert(CacheMapPair(object_ptr, move(ptr))).first;
      } else {
        hit = new_cache.insert(CacheMapPair(object_ptr, unique_ptr<ObjectCache>(
                                              new ObjectCache(object)))).first;
      }
    }
    Point3DVector &points = hit->second->points;
    TriangleVector &triangles = hit->second->triangles;
    std::vector<bool> &point_flags = hit->second->point_flags;
    std::vector<size_t> &material_indexes = hit->second->material_indexes;
    int o_size = object->positioned_points().size();
    points.resize(o_size);
    point_flags.assign(o_size, false);
    triangles.clear();
    triangles.reserve(object->model()->polygons().size() / 2);
    material_indexes.clear();
    MaterialIndexVector::const_iterator m_i;
    if (object->model()->material_indexes()) {
      material_indexes.reserve(triangles.size());
      m_i = object->model()->material_indexes()->begin();
    }

    auto p_i = object->model()->polygons().begin();
    auto n_i = object->positioned_polygon_normals().begin();
    for (auto pe = object->model()->polygons().end(); p_i != pe; ++p_i, ++n_i) {
#ifndef NO_NORMAL_FACE_CLIPPING
      if (Point3D::ScalarMul(normal, *n_i) <= 0) {
#endif
        const IndexedTriangle &p = *p_i;
        triangles.push_back(p);
        if (object->model()->material_indexes()) {
          material_indexes.push_back(*m_i);
          ++m_i;
        }
        for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
          int point = p.points[i];
          if (!point_flags[point]) {
            Point3D dest = transform * object->positioned_points()[point];
            myfloat k = viewer_distance_ / (viewer_distance_ + dest.z);
            // Perspective transformation
            dest.x = dest.x * k * scale_ + surface_->width() / 2.0;
            dest.y = dest.y * k * scale_ + surface_->height() / 2.0;
            points[point] = move(dest);
            point_flags[point] = true;
          }
        }
#ifndef NO_NORMAL_FACE_CLIPPING
      }
#endif
    }

    if (trace_) {
      int result = point_tracer_.TraceNext(triangles, points);
      if (result != -1) {
        traced_object_ = object;
      }
    }
  }

  cache_ = std::move(new_cache);
}

void Rasterizer::SecondPass() {
  z_buffer_.set_size(surface_->width(), surface_->height());
  z_buffer_.Clear();
  if (trace_) {
    point_tracer_.Reset();
  }
  surface_painter_.StartDrawing();
  for (auto &object : cache_) {
    std::shared_ptr<SceneObject> model = object.second->object.lock();
    auto t_i = object.second->triangles.begin();
    if (model->model()->material_indexes()) {
      auto m_i = object.second->material_indexes.begin();
      for (auto t_end = object.second->triangles.end(); t_i != t_end; ++t_i) {
        DrawTriangle(*t_i, object.second->points, *model->model(), model->model()->materials()[*m_i]);
      }
    } else {
      for (auto t_end = object.second->triangles.end(); t_i != t_end; ++t_i) {
        DrawTriangle(*t_i, object.second->points, *model->model(), model->model()->materials()[0]);
      }
    }
  }
  surface_painter_.FinishDrawing();
}

void Rasterizer::set_scene(const Scene *scene) {
  scene_ = scene;
}

void Rasterizer::set_camera(const Position *camera) {
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

void Rasterizer::set_trace(const bool trace) {
  trace_ = trace;
}

void Rasterizer::set_view_limit(const myfloat view_limit) {
  view_limit_ = view_limit;
}

void Rasterizer::set_trace_point(const unsigned x, const unsigned y) {
  point_tracer_.set_point(x, y);
}

void Rasterizer::ClearCache() {
  cache_.clear();
}


Rasterizer::ObjectCache::ObjectCache(const std::weak_ptr<SceneObject> &object_)
 : object(object_) {}
