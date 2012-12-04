#include "common/math.h"
#include "common/debug.h"
#include "common/exception.h"
#include "triangletraverser.h"
#include "rasterizer.h"

#define NO_NORMAL_FACE_CLIPPING
#define NO_LIGHTING

using namespace std;
using namespace sdlobj;

Rasterizer::Rasterizer(const std::shared_ptr<Context> &context) :
 ContextUser(context), cache_(new TransformedObjectMap()), z_buffer_(0, 0) {
  context->transformed_objects = cache_;
}

/* These functions are safe to call with result = a | b */

template<class T> class Less {
 public:
  inline static bool Compare(const T &a, const T &b) {
    return a < b;
  }
};

template<class T> class Greater {
 public:
  inline static bool Compare(const T &a, const T &b) {
    return a > b;
  }
};

template<class Comparer> class XClipper {
 public:
  inline static bool Compare(const Point3D &point, const myfloat value) {
    return Comparer::Compare(point.x, value);
  }

  inline static void Intersect(const Point3D &a, const Point3D &b, Point3D &result, const myfloat value) {
    myfloat k = (value - a.x) / (b.x - a.x);
    result.x = value;
    result.y = k * (b.y - a.y) + a.y;
    result.z = k * (b.z - a.z) + a.z;
  }
};

template<class Comparer> class YClipper {
 public:
  inline static bool Compare(const Point3D &point, const myfloat value) {
    return Comparer::Compare(point.y, value);
  }

  inline static void Intersect(const Point3D &a, const Point3D &b, Point3D &result, const myfloat value) {
    myfloat k = (value - a.y) / (b.y - a.y);
    result.x = k * (b.x - a.x) + a.x;
    result.y = value;
    result.z = k * (b.z - a.z) + a.z;
  }
};

template<class Comparer> class ZClipper {
 public:
  inline static bool Compare(const Point3D &point, const myfloat value) {
    return Comparer::Compare(point.z, value);
  }

  inline static void Intersect(const Point3D &a, const Point3D &b, Point3D &result, const myfloat value) {
    myfloat k = (value - a.z) / (b.z - a.z);
    result.x = k * (b.x - a.x) + a.x;
    result.y = k * (b.y - a.y) + a.y;
    result.z = value;
  }
};

template<class Clipper> void Clip(IndexedTriangle &triangle, IndexedTriangle *triangles, size_t &triangles_num,
           Point3D *points, size_t &points_num, const myfloat value) {
  int good_indexes[IndexedTriangle::kPointsSize];
  int bad_indexes[IndexedTriangle::kPointsSize];
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
      triangle.Set(good_indexes[0], good_indexes[1], points_num);
      triangles[triangles_num++].Set(good_indexes[1], points_num, points_num + 1);
      Clipper::Intersect(points[good_indexes[0]], points[bad_indexes[0]], points[points_num++], value);
      Clipper::Intersect(points[good_indexes[1]], points[bad_indexes[0]], points[points_num++], value);
      break;
    case 2:
      // trim part
      triangle.Set(good_indexes[0], points_num, points_num + 1);
      Clipper::Intersect(points[good_indexes[0]], points[bad_indexes[1]], points[points_num++], value);
      Clipper::Intersect(points[good_indexes[0]], points[bad_indexes[0]], points[points_num++], value);
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
const size_t kPolygonsSize = 18; // 33
// One point for each clip can become unusable
const size_t kPointsSize = 53; // 108

template <class Clipper> bool ClipTriangles(IndexedTriangle *triangles, size_t &triangles_num,
                                            Point3D *points, size_t &points_num, const myfloat &value) {
  size_t curr_triangles = triangles_num;
  for (size_t i = 0; i < curr_triangles; ++i)
    if (triangles[i].points[0] != -1)
      Clip<Clipper>(triangles[i], triangles, triangles_num, points, points_num, value);
#if DEBUG_LEVEL >= 4
  AssertMsg(triangles_num <= kPolygonsSize, "There are more polygons than expected from clipping.");
  AssertMsg(points_num <= kPointsSize, "There are more points than expected from clipping.");
#endif
  return triangles_num == 1 && triangles[0].points[0] == -1;
}

template <class Integral = myfloat> class NormalsTraversable : virtual public Traversable<Integral> {
 public:
  typedef Point3D DataType;

  class HorizontalTraversable : virtual public Traversable<Integral>::template HorizontalTraversable<Point3D> {
   public:
    inline HorizontalTraversable(const ScreenLine<Integral> &la, const NormalsTraversable<Integral> &a,
                                 const ScreenLine<Integral> &lb, const NormalsTraversable<Integral> &b) : n_(a.n()) {
      myfloat dx = (Integral)lb.x() - (Integral)la.x();
      if (dx != 0) {
        dn_.x = (b.n().x - a.n().x) / dx;
        dn_.y = (b.n().y - a.n().y) / dx;
        dn_.z = (b.n().z - a.n().z) / dx;
      }
    }

    inline void Advance() {
      n_ += dn_;
    }

    inline void Advance(const Integral value) {
      n_ += dn_ * value;
    }

    inline bool Process(Point3D *context, const Integral, const Integral) {
      *context = n_;
      return true;
    }

    inline const Point3D &n() const {
      return n_;
    }

    inline const Point3D &dn() const {
      return dn_;
    }

   private:
    Point3D n_;
    Point3D dn_;
  };

  inline NormalsTraversable() = default;

  inline NormalsTraversable(const ScreenLine<Integral> &line, const DataType &a, const DataType &b) : n_(a) {
    myfloat dy = line.fy() - line.y();
    if (dy != 0) {
      dn_.x = (b.x - a.x) / dy;
      dn_.y = (b.y - a.y) / dy;
      dn_.z = (b.z - a.z) / dy;
    }
  }

  inline void Advance() {
    n_ += dn_;
  }

  inline void Advance(const Integral value) {
    n_ += dn_ * value;
  }

  inline const Point3D &n() const {
    return n_;
  }

  inline const Point3D &dn() const {
    return dn_;
  }

 private:
  Point3D n_;
  Point3D dn_;
};

struct ZTraversableData {
  myfloat z;
};

struct UVTraversableData : virtual public ZTraversableData {
  Point2D uv;
};

template <class Integral = myfloat> class UVCoordsTraversable : virtual public Traversable<Integral>,
    virtual public ZTraversable<Integral> {
 public:
  typedef UVTraversableData DataType;
  typedef typename ZTraversable<Integral>::HorizontalTraversable ZTraversableHorizontal;
  typedef typename Traversable<Integral>::template HorizontalTraversable<Point2D> TraversableHorizontal;

  class HorizontalTraversable : public TraversableHorizontal, public virtual ZTraversableHorizontal {
   public:
    inline HorizontalTraversable(const ScreenLine<Integral> &la, const UVCoordsTraversable &a,
                                 const ScreenLine<Integral> &lb, const UVCoordsTraversable &b) :
        ZTraversableHorizontal(la, a, lb, b), uv_z_(a.uv_z()) {
      myfloat dx = (Integral)lb.x() - (Integral)la.x();
      if (dx != 0) {
        duv_z_.x = (b.uv_z().x - uv_z_.x) / dx;
        duv_z_.y = (b.uv_z().y - uv_z_.y) / dx;
      }
    }

    inline void Advance() {
      uv_z_ += duv_z_;
    }

    inline void Advance(const Integral value) {
      uv_z_ += duv_z_ * value;
    }

    inline bool Process(Point2D *context, const Integral, const Integral) {
      *context = GetUV();
      return true;
    }

    inline Point2D GetUV() const {
      Point2D uv;
      uv.x = uv_z_.x * this->z();
      uv.y = uv_z_.y * this->z();
      return uv;
    }

    inline const Point2D &uv_z() const {
      return uv_z_;
    }

    inline const Point2D &duv_z() const {
      return duv_z_;
    }

   private:
    Point2D uv_z_;
    Point2D duv_z_;
  };

  inline UVCoordsTraversable() = default;

  inline UVCoordsTraversable(const ScreenLine<Integral> &line, const DataType &a, const DataType &b) :
      ZTraversable<Integral>(line, a.z, b.z) {
    myfloat iz = 1 / this->z();
    uv_z_.x = a.uv.x * iz;
    uv_z_.y = a.uv.y * iz;
    myfloat dy = line.fy() - line.y();
    if (dy != 0) {
      myfloat izb = 1 / b.z;
      duv_z_.x = (b.uv.x * izb - uv_z_.x) / dy;
      duv_z_.y = (b.uv.y * izb - uv_z_.y) / dy;
    }
  }

  inline void Advance() {
    uv_z_ += duv_z_;
  }

  inline void Advance(const Integral value) {
    uv_z_ += duv_z_ * value;
  }

  inline Point2D GetUV() const {
    Point2D uv;
    uv.x = uv_z_.x * this->z();
    uv.y = uv_z_.y * this->z();
    return uv;
  }

  inline const Point2D &uv_z() const {
    return uv_z_;
  }

  inline const Point2D &duv_z() const {
    return duv_z_;
  }

 private:
  Point2D uv_z_;
  Point2D duv_z_;
};

struct RasterizerTraversableData : virtual public ZTraversableData {
#ifndef NO_LIGHTING
 // Point3D normal;
#endif
};

template <class Integral = unsigned> class RasterizerTraversable : public Traversable<Integral>,
    public virtual ZTraversable<Integral>
#ifndef NO_LIGHTING
    , public NormalsTraversable
#endif
{
 public:
  struct TraversableContext {
    const Material *material;
    const Surface *surface;
    ZBuffer *z_buffer;
    SurfacePainter *surface_painter;
  };

  typedef RasterizerTraversableData DataType;
  typedef typename ZTraversable<Integral>::HorizontalTraversable ZTraversableHorisontal;
  typedef typename Traversable<Integral>::template HorizontalTraversable<TraversableContext> TraversableHorisontal;

  class HorizontalTraversable : public TraversableHorisontal, public virtual ZTraversableHorisontal
#ifndef NO_LIGHTING
      , public NormalsTraversable::HorizontalTraversable
#endif
  {
   public:
    typedef TraversableContext ContextType;

    inline HorizontalTraversable(const ScreenLine<Integral> &a, const RasterizerTraversable &da,
                                 const ScreenLine<Integral> &b, const RasterizerTraversable &db) :
      ZTraversableHorisontal(a, da, b, db)
#ifndef NO_LIGHTING
      , NormalsTraversable::HorizontalTraversable(a, da, b, db)
#endif
    {}

    virtual inline bool Process(TraversableContext *context, const Integral x, const Integral y) {
      sdlobj::Color color = context->material->color();
      color = ProcessColor(context, color, x, y);
      Uint32 pixel = context->surface->ColorToPixel(color);
      SetPixel(context, x, y, this->z(), pixel);
      return true;
    }

    inline sdlobj::Color ProcessColor(TraversableContext *context, const sdlobj::Color &color, const unsigned x, const unsigned y) {
#ifdef NO_LIGHTING
      return color;
#endif
    }

    static inline void SetPixel(TraversableContext *context, const unsigned x, const unsigned y, const myfloat z, const Uint32 pixel) {
      if (context->z_buffer->Check(x, y, z)) {
        context->surface_painter->SetPixel(x, y, pixel);
      }
    }

    inline void Advance() {
      ZTraversableHorisontal::Advance();
#ifndef NO_LIGHTING
      NormalsTraversable::HorizontalTraversable::Advance();
#endif
    }

    inline void Advance(const Integral value) {
      ZTraversableHorisontal::Advance(value);
#ifndef NO_LIGHTING
      NormalsTraversable::HorizontalTraversable::Advance(value);
#endif
    }
  };

  RasterizerTraversable(const ScreenLine<Integral> &line, const DataType &a, const DataType &b) :
      ZTraversable<Integral>(line, a.z, b.z)
#ifndef NO_LIGHTING
    , NormalsTraversable(line, a.normal, b.normal)
#endif
  {}

  inline void Advance() {
    ZTraversable<Integral>::Advance();
#ifndef NO_LIGHTING
    NormalsTraversable::Advance();
#endif
  }

  inline void Advance(const Integral value) {
    ZTraversable<Integral>::Advance(value);
#ifndef NO_LIGHTING
    NormalsTraversable::Advance(value);
#endif
  }
};

struct RasterizerTexturedTraversableData : public RasterizerTraversableData,
    public UVTraversableData {
};

template <class Integral = unsigned> struct RasterizerTexturedTraversable : public RasterizerTraversable<Integral>,
    public UVCoordsTraversable<Integral> {
 public:
  struct TraversableContext : public RasterizerTraversable<Integral>::TraversableContext {
    const SurfacePainter *texture;
  };

  typedef RasterizerTexturedTraversableData DataType;
  typedef typename UVCoordsTraversable<Integral>::HorizontalTraversable UVTraversableHorizontal;
  typedef typename RasterizerTraversable<Integral>::HorizontalTraversable RasterizerHorizontal;
  typedef typename ZTraversable<Integral>::HorizontalTraversable ZTraversableHorisontal;

  class HorizontalTraversable : public RasterizerHorizontal,
      public UVTraversableHorizontal, virtual public ZTraversableHorisontal {
   public:
    typedef TraversableContext ContextType;

    inline HorizontalTraversable(const ScreenLine<Integral> &a, const RasterizerTexturedTraversable &da,
                                 const ScreenLine<Integral> &b, const RasterizerTexturedTraversable &db) :
      ZTraversableHorisontal(a, da, b, db),
      RasterizerHorizontal(a, da, b, db),
      UVTraversableHorizontal(a, da, b, db) {}

    virtual inline bool Process(TraversableContext *context, const Integral x, const Integral y) {
      Point2D uv;// = this->GetUV();
      uv.x = 0;
      uv.y = 0;
      Uint32 t_pixel = context->texture->GetPixel(uv.x, uv.y);
      sdlobj::Color color = context->material->texture()->PixelToColor(t_pixel);
      color = this->ProcessColor(context, color, x, y);
      Uint32 pixel = context->surface->ColorToPixel(color);
      this->SetPixel(context, x, y, this->z(), pixel);
      return true;
    }

    inline void Advance() {
      RasterizerHorizontal::Advance();
      UVTraversableHorizontal::Advance();
    }

    inline void Advance(const Integral value) {
      RasterizerHorizontal::Advance(value);
      UVTraversableHorizontal::Advance(value);
    }
  };

  inline RasterizerTexturedTraversable(const ScreenLine<Integral> &line, const DataType &a, const DataType &b) :
      ZTraversable<Integral>(line, a.z, b.z), RasterizerTraversable<Integral>(line, a, b),
      UVCoordsTraversable<Integral>(line, a, b) {}

  inline void Advance() {
    RasterizerTraversable<Integral>::Advance();
    UVCoordsTraversable<Integral>::Advance();
  }

  inline void Advance(const Integral value) {
    RasterizerTraversable<Integral>::Advance(value);
    UVCoordsTraversable<Integral>::Advance(value);
  }
};

template <bool kTextures> void DrawTriangle(const size_t tri_i, const TransformedObject &cacheobject,
        const SceneObject &object, RasterizerTexturedTraversable<unsigned>::TraversableContext *context) {
  size_t triangles_size = 1;
  size_t points_size = IndexedTriangle::kPointsSize;

  IndexedTriangle triangles_buf[kPolygonsSize];
  Point3D points_buf[kPointsSize];

  const IndexedTriangle &tri = object.model()->polygons()[tri_i];
  for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
    triangles_buf[0].points[i] = i;
  }
  for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
    points_buf[i] = cacheobject.points[tri.points[i]];
  }
  
  // clipping
  if (ClipTriangles<ZClipper<Less<myfloat>>>(triangles_buf, triangles_size, points_buf,
                                             points_size, 0)) return;
  if (ClipTriangles<XClipper<Less<myfloat>>>(triangles_buf, triangles_size, points_buf,
                                             points_size, 0)) return;
  if (ClipTriangles<XClipper<Greater<myfloat>>>(triangles_buf, triangles_size, points_buf,
                                                       points_size, context->surface->width() - 1)) return;
  if (ClipTriangles<YClipper<Less<myfloat>>>(triangles_buf, triangles_size, points_buf,
                                             points_size, 0)) return;
  if (ClipTriangles<YClipper<Greater<myfloat>>>(triangles_buf, triangles_size, points_buf,
                                                       points_size, context->surface->height() - 1)) return;

  RasterizerTexturedTraversableData data[kPointsSize];
  for (size_t i = 0; i < points_size; ++i) {
    data[i].z = points_buf[i].z;
  }

#ifndef NO_LIGHTING
  for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
    data[i].normal = object.model()->vertex_normals()[triangles_buf[0].points[i]];
  }
  bool normal_found[points_size];
  if (points_size > 3) {
    TriangleTraverser<NormalsTraversable<myfloat>> traverser(points_buf[0], data[0].normal,
                                                    points_buf[1], data[1].normal,
                                                    points_buf[2], data[2].normal);
    memset((bool *)&normal_found, true, 3 * sizeof(bool));
    memset((bool *)&normal_found + 3, false, (points_size - 3) * sizeof(bool));
    for (size_t i = 0; i < triangles_size; ++i) {
      IndexedTriangle &pol = triangles_buf[i];
      if (pol.points[0] == -1) continue;
      for (size_t j = 0; j < IndexedTriangle::kPointsSize; ++j) {
        size_t pt = pol.points[j];
        if (!normal_found[pt]) {
          traverser.Point(&data[pt].normal, points_buf[pt].x, points_buf[pt].y);
          normal_found[pt] = true;
        }
      }
    }
  }
#endif

  if (kTextures) {
    for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
      data[i].uv = (*object.model()->uv_coords())[tri.points[i]];
      data[i].uv.x *= context->texture->surface()->width() - 1;
      data[i].uv.y *= context->texture->surface()->height() - 1;
    }
    if (points_size > 3) {
      TriangleTraverser<UVCoordsTraversable<myfloat>> traverser(points_buf[0], data[0],
                                                       points_buf[1], data[1],
                                                       points_buf[2], data[2]);
#ifndef NO_LIGHTING
      for (size_t i = IndexedTriangle::kPointsSize; i < points_size; ++i) {
        if (normal_found[i]) {
          traverser.Point(&data[i].uv, points_buf[i].x, points_buf[i].y);
        }
      }
#else
      bool normal_found[points_size];
      memset((bool *)&normal_found, false, 3 * sizeof(bool));
      memset((bool *)&normal_found + 3, true, (points_size - 3) * sizeof(bool));
      for (size_t i = 0; i < triangles_size; ++i) {
        IndexedTriangle &pol = triangles_buf[i];
        if (pol.points[0] == -1) continue;
        for (size_t j = 0; j < IndexedTriangle::kPointsSize; ++j) {
          size_t pt = pol.points[j];
          if (!normal_found[pt]) {
            traverser.Point(&data[pt].uv, points_buf[pt].x, points_buf[pt].y);
            normal_found[pt] = true;
          }
        }
      }
#endif
    }

    for (size_t i = 0; i < triangles_size; ++i) {
      if (triangles_buf[i].points[0] == -1) continue;
      const int *pts = triangles_buf[i].points;
      TriangleTraverser<RasterizerTexturedTraversable<>>::OneTraverse(context,
          points_buf[pts[0]], data[pts[0]],
          points_buf[pts[1]], data[pts[1]],
          points_buf[pts[2]], data[pts[2]]);
    }
  } else {
    for (size_t i = 0; i < triangles_size; ++i) {
      if (triangles_buf[i].points[0] == -1) continue;
      const int *pts = triangles_buf[i].points;
      TriangleTraverser<RasterizerTraversable<>>::OneTraverse(context,
          points_buf[pts[0]], data[pts[0]],
          points_buf[pts[1]], data[pts[1]],
          points_buf[pts[2]], data[pts[2]]);
    }
  }
}

template <bool kTextures> void Rasterizer::TransformObject(const std::shared_ptr<SceneObject> &object, const Matrix4 &transform,
                                                           TransformedObjectMap &new_cache, const Point3D &normal) {
  SceneObject *object_ptr = object.get();
  Model *model = object->model().get();
  TransformedObjectMap::iterator hit = new_cache.find(object_ptr);
  if (hit == new_cache.end()) {
    TransformedObjectMap::iterator old_hit = cache_->find(object_ptr);
    if (old_hit != cache_->end()) {
      std::unique_ptr<TransformedObject> ptr;
      ptr.swap(old_hit->second);
      hit = new_cache.insert(TransformedObjectPair(object_ptr, move(ptr))).first;
    } else {
      hit = new_cache.insert(TransformedObjectPair(object_ptr, unique_ptr<TransformedObject>(
                                                   new TransformedObject(object)))).first;
    }
  }
  hit->second->material_overlay.reset();
  Point3DVector &points = hit->second->points;
  std::vector<bool> &point_flags = hit->second->point_flags;
  std::vector<size_t> &triangle_indexes = hit->second->triangle_indexes;
  int o_size = object->positioned_points().size();
  points.resize(o_size);
  point_flags.assign(o_size, false);
  triangle_indexes.clear();
  triangle_indexes.reserve(model->polygons().size() / 2);

  auto p_i = model->polygons().begin();
  auto n_i = object->positioned_polygon_normals().begin();
  size_t i = 0;
  for (auto pe = model->polygons().end(); p_i != pe; ++p_i, ++n_i, ++i) {
#ifndef NO_NORMAL_FACE_CLIPPING
    if (Point3D::ScalarMul(normal, *n_i) <= 0) {
#endif
      triangle_indexes.push_back(i);
      int indexes[IndexedTriangle::kPointsSize];
      for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
        indexes[i] = p_i->points[i];
      }
      for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
        if (!point_flags[indexes[i]]) {
          Point3D dest = transform * object->positioned_points()[indexes[i]];
          context()->camera.PerspectiveTransform(dest);
          points[indexes[i]] = move(dest);
          point_flags[indexes[i]] = true;
        }
      }
#ifndef NO_NORMAL_FACE_CLIPPING
    }
#endif
  }
}

void Rasterizer::PreRenderStep() {
  static const myfloat system_transform_m[] = {0, -1, 0, 0,
                                               0, 0, -1, 0,
                                               1, 0, 0, 0,
                                               0, 0, 0, 1};

  Matrix4 transform(system_transform_m);
  transform *= context()->camera.GetMatrixTo();
#ifndef NO_NORMAL_FACE_CLIPPING
  Point3D normal = context()->camera.GetRotateMatrixFrom() * Position::kCameraDirection;
#else
  Point3D normal;
#endif

  TransformedObjectMap new_cache;

  for (auto &object : context()->scene.objects()) {
    if (object->model()->uv_coords()) {
      TransformObject<true>(object, transform, new_cache, normal);
    } else {
      TransformObject<false>(object, transform, new_cache, normal);
    }
  }

  *cache_ = std::move(new_cache);
}

void Rasterizer::Paint(sdlobj::Surface &surface) {
  z_buffer_.set_size(context()->camera.width(), context()->camera.height());
  z_buffer_.Clear();

  SurfacePainter surface_painter(&surface);

  RasterizerTexturedTraversable<>::TraversableContext context;
  context.surface = &surface;
  context.z_buffer = &z_buffer_;
  context.surface_painter = &surface_painter;

  surface_painter.StartDrawing();
  for (auto &cacheobject : *cache_) {
    const TransformedObject &transformed = *cacheobject.second;
    std::shared_ptr<SceneObject> object = transformed.object.lock();
    if (transformed.material_overlay) {
      MaterialOverlayMap *map = transformed.material_overlay.get();
      auto m_end = map->end();
      for (const auto &t : transformed.triangle_indexes) {
        auto m_i = map->find(t);
        const Material *material;
        if (m_i == m_end) {
          material = object->model()->materials()[t].get();
        } else {
          material = m_i->second.get();
        }
        context.material = material;
        if (material->texture()) {
          SurfacePainter texture_painter(material->texture().get());
          texture_painter.StartDrawing();
          context.texture = &texture_painter;
          DrawTriangle<true>(t, transformed, *object, &context);
        } else {
          DrawTriangle<false>(t, transformed, *object, &context);
        }
      }
    } else {
      for (const auto &t : transformed.triangle_indexes) {
        const Material &material = *object->model()->materials()[t];
        context.material = &material;
        if (material.texture()) {
          SurfacePainter texture_painter(material.texture().get());
          texture_painter.StartDrawing();
          context.texture = &texture_painter;
          DrawTriangle<true>(t, transformed, *object, &context);
        } else {
          DrawTriangle<false>(t, transformed, *object, &context);
        }
      }
    }
  }
}

void Rasterizer::set_view_limit(const myfloat view_limit) {
  view_limit_ = view_limit;
}

void Rasterizer::Clear() {
  cache_->clear();
}
