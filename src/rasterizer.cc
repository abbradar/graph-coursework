#include <algorithm>
#include "common/math.h"
#include "common/debug.h"
#include "common/exception.h"
#include "surfacepainterfactory.h"
#include "triangletraverser.h"
#include "config.h"
#include "rasterizer.h"

using namespace std;
using namespace sdlobj;

Rasterizer::Rasterizer(const std::shared_ptr<Context> &context) :
 ContextUser(context), cache_(new TransformedObjectMap()), z_buffer_(0, 0) {
  context->transformed_objects = cache_;
}

/* These functions are safe to call with result = a | b */

template <class T> class Less {
 public:
  inline static bool Compare(const T &a, const T &b) {
    return a < b;
  }
};

template <class T> class Greater {
 public:
  inline static bool Compare(const T &a, const T &b) {
    return a > b;
  }
};

template <class Comparer> class XClipper {
 public:
  inline static bool Compare(const Vector3 &point, const myfloat value) {
    return Comparer::Compare(point.x(), value);
  }

  inline static void Intersect(const Vector3 &a, const Vector3 &b, Vector3 &result, const myfloat value) {
    myfloat k = (value - a.x()) / (b.x() - a.x());
    result.x() = value;
    result.y() = k * (b.y() - a.y()) + a.y();
    result.z() = k * (b.z() - a.z()) + a.z();
  }
};

template <class Comparer> class YClipper {
 public:
  inline static bool Compare(const Vector3 &point, const myfloat value) {
    return Comparer::Compare(point.y(), value);
  }

  inline static void Intersect(const Vector3 &a, const Vector3 &b, Vector3 &result, const myfloat value) {
    myfloat k = (value - a.y()) / (b.y() - a.y());
    result.x() = k * (b.x() - a.x()) + a.x();
    result.y() = value;
    result.z() = k * (b.z() - a.z()) + a.z();
  }
};

template <class Comparer> class ZClipper {
 public:
  inline static bool Compare(const Vector3 &point, const myfloat value) {
    return Comparer::Compare(point.z(), value);
  }

  inline static void Intersect(const Vector3 &a, const Vector3 &b, Vector3 &result, const myfloat value) {
    myfloat k = (value - a.z()) / (b.z() - a.z());
    result.x() = k * (b.x() - a.x()) + a.x();
    result.y() = k * (b.y() - a.y()) + a.y();
    result.z() = value;
  }
};

template <class Clipper> void Clip(IndexedTriangle &triangle, IndexedTriangle *triangles, size_t &triangles_num,
           Vector3 *points, size_t &points_num, const myfloat value) {
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

template <class Clipper> bool Clip(Vector3 &a, Vector3 &b, const myfloat value) {
  if (Clipper::Compare(a, value)) {
    if (Clipper::Compare(b, value)) return false;
    Clipper::Intersect(a, b, a, value);
  } else if (Clipper::Compare(b, value)) {
    if (Clipper::Compare(a, value)) return false;
    Clipper::Intersect(a, b, b, value);
  }

  return true;
}

// Maximum number of splitted triangles after clipping
const size_t kPolygonsSize = 18; // 33
// One point for each clip can become unusable
const size_t kPointsSize = 53; // 108

template <class Clipper> bool ClipTriangles(IndexedTriangle *triangles, size_t &triangles_num,
                                            Vector3 *points, size_t &points_num, const myfloat &value) {
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

#if defined(PHONG_SHADING)

struct FullLightingData : public LightingData {
  Vector3 normal;
};

template <class Integral = myfloat> class LightingTraversable : virtual public Traversable<Integral> {
 public:
  typedef FullLightingData DataType;

  class HorizontalTraversable : virtual public Traversable<Integral>::template HorizontalTraversable<DataType> {
   public:
    inline HorizontalTraversable(FullLightingData *, const ScreenLine<Integral> &la, const LightingTraversable<Integral> &a,
                                 const ScreenLine<Integral> &lb, const LightingTraversable<Integral> &b,
                                 const myfloat dx) :
        normal_(a.normal()), viewer_(a.viewer()), sources_size_(a.sources_size()),
        sources_(sources_size_) {
      memcpy(sources_.data(), a.sources().data(), sources_size_ * sizeof(LightingSourceData));
      if (dx != 0) {
        dnormal_ = (b.normal() - normal_) / dx;
        dviewer_ = (b.viewer() - viewer_) / dx;
        dsources_ = RuntimeArray(sources_size_);
        for (size_t i = 0; i < sources_size_; ++i) {
          dsources_[i].direction = (b.sources()[i].direction - sources_[i].direction) / dy;
          dsources_[i].reflection = (b.sources()[i].reflection - sources_[i].reflection) / dy;
        }
      } else {
        dsources_ = nullptr;
      }
    }

    // this should run only once, so - some dirty "hack"
    inline bool Process(FullLightingData *context, const Integral, const Integral) {
      context->viewer = viewer_;
      context->normal = normal_;
      context->sources = std::move(sources_);
      return true;
    }

    inline void Advance() {
      normal_ += dnormal_;
      viewer_ += dviewer_;
      for (size_t i = 0; i < sources_size_; ++i) {
        sources_[i].direction += dsources_[i].direction;
        sources_[i].reflection += dsources_[i].reflection;
      }
    }

    inline void Advance(const Integral value) {
      normal_ += dnormal_ * value;
      viewer_ += dviewer_ * value;
      for (size_t i = 0; i < sources_size_; ++i) {
        sources_[i].direction += dsources_[i].direction * value;
        sources_[i].reflection += dsources_[i].reflection * value;
      }
    }

    inline const Vector3 &normal() const {
      return normal_;
    }

    inline const Vector3 &viewer() const {
      return viewer_;
    }

    inline size_t sources_size() const {
      return sources_size_;
    }

    inline RuntimeArray &sources() {
      return sources_;
    }

   private:
    Vector3 normal_;
    Vector3 dnormal_;
    Vector3 viewer_;
    Vector3 dviewer_;
    RuntimeArray sources_;
    RuntimeArray dsources_;
    size_t sources_size_;
  };

  inline LightingTraversable() = default;

  inline LightingTraversable(const ScreenLine<Integral> &line, const myfloat dy, const DataType &a, const DataType &b) :
      normal_(a.normal), viewer_(a.viewer), sources_size_(a.sources_size),
      sources_(sources_size_) {
    memcpy(sources_, a.sources_, sources_size_ * sizeof(LightingSourceData));
    if (dy != 0) {
      dnormal_ = (b.normal - normal_) / dy;
      dviewer_ = (b.viewer - viewer_) / dy;
      dsources_ = new LightingSourceData[sources_size_];
      for (size_t i = 0; i < sources_size_; ++i) {
        dsources_[i].direction = (b.sources[i].direction - sources_[i].direction) / dy;
        dsources_[i].reflection = (b.sources[i].reflection - sources_[i].reflection) / dy;
      }
    } else {
      dsources_ = nullptr;
    }
  }

  inline void Advance() {
    normal_ += dnormal_;
    viewer_ += dviewer_;
    for (size_t i = 0; i < sources_size_; ++i) {
      sources_[i].direction += dsources_[i].direction;
      sources_[i].reflection += dsources_[i].reflection;
    }
  }

  inline void Advance(const Integral value) {
    normal_ += dnormal_ * value;
    viewer_ += dviewer_ * value;
    for (size_t i = 0; i < sources_size_; ++i) {
      sources_[i].direction += dsources_[i].direction * value;
      sources_[i].reflection += dsources_[i].reflection * value;
    }
  }

  inline const Vector3 &normal() const {
    return normal_;
  }

  inline const Vector3 &viewer() const {
    return viewer_;
  }

  inline size_t sources_size() const {
    return sources_size_;
  }

  inline const LightingSourceData *sources() const {
    return sources_;
  }

  inline RuntimeArray &sources() {
    return sources_;
  }

 private:
  Vector3 normal_;
  Vector3 dnormal_;
  Vector3 viewer_;
  Vector3 dviewer_;
  size_t sources_size_;
  RuntimeArray<LightingSourceData> sources_;
  RuntimeArray<LightingSourceData> dsources_;
};

#elif defined(GOURAUD_SHADING)

template <class Integral = myfloat> class ColorTraversable : virtual public Traversable<Integral> {
 public:
  typedef Vector3 DataType;

  class HorizontalTraversable : virtual public Traversable<Integral>::template HorizontalTraversable<DataType> {
   public:
    inline HorizontalTraversable(LightingData *, const ScreenLine<Integral> &la, const ColorTraversable<Integral> &a,
                                 const ScreenLine<Integral> &lb, const ColorTraversable<Integral> &b,
                                 const myfloat dx) : color_(a.color()) {
      if (dx != 0) {
        dcolor_ = (b.color() - color_) / dx;
      }
    }

    inline bool Process(DataType *context, const Integral, const Integral) {
      *context = color_;
      return true;
    }

    inline void Advance() {
      color_ += dcolor_;
    }

    inline void Advance(const Integral value) {
      color_ += dcolor_ * value;
    }

    inline const Vector3 &color() const {
      return color_;
    }

   private:
    Vector3 color_;
    Vector3 dcolor_;
  };

  inline LightingTraversable() = default;

  inline LightingTraversable(const ScreenLine<Integral> &line, const myfloat dy, const DataType &a, const DataType &b) :
      color_(a) {
    if (dy != 0) {
      dcolor_ = (b - a) / dy;
    }
  }

  inline void Advance() {
    color_ += dcolor_;
  }

  inline void Advance(const Integral value) {
    color_ += dcolor_ * value;
  }

  inline const Vector3 &color() const {
    return color_;
  }

 private:
  Vector3 color_;
  Vector3 dcolor_;
};

#endif

struct ZTraversableData {
  myfloat z;
};

struct TextureTraversableData : virtual public ZTraversableData {
  Vector2 uv;
};

template <class Integral = myfloat> class TextureTraversable : virtual public Traversable<Integral>,
    virtual public ZTraversable<Integral> {
 public:
  typedef TextureTraversableData DataType;
  typedef typename ZTraversable<Integral>::HorizontalTraversable ZTraversableHorizontal;
  typedef typename Traversable<Integral>::template HorizontalTraversable<Vector2> TraversableHorizontal;

  class HorizontalTraversable : public TraversableHorizontal, public virtual ZTraversableHorizontal {
   public:
    inline HorizontalTraversable(Vector2 *, const ScreenLine<Integral> &la, const TextureTraversable &a,
                                 const ScreenLine<Integral> &lb, const TextureTraversable &b, const myfloat dx) :
      ZTraversableHorizontal(la, a, lb, b, dx), uv_z_(a.uv_z()) {
      if (dx != 0) {
        duv_z_ = (b.uv_z() - uv_z_) / dx;
      }
    }

    inline const Vector2 &uv_z() const {
      return uv_z_;
    }

    inline void Advance() {
      uv_z_ += duv_z_;
    }

    inline void Advance(const Integral value) {
      uv_z_ += duv_z_ * value;
    }

    inline bool Process(Vector2 *context, const Integral, const Integral) {
      *context = GetUV();
      return true;
    }

    inline Vector2 GetUV() const {
#ifndef AFFINE_TEXTURES
      return uv_z_ * this->z();
#else
      return uv_z_;
#endif
    }

   private:
    Vector2 uv_z_;
    Vector2 duv_z_;
  };

  inline TextureTraversable() = default;

  inline TextureTraversable(const ScreenLine<Integral> &line, const myfloat dy,
                            const DataType &a, const DataType &b) :
      ZTraversable<Integral>(line, dy, a.z, b.z) {
#ifndef AFFINE_TEXTURES
    myfloat iz = 1 / this->z();
    uv_z_ = a.uv * iz;
#else
    uv_z_ = a.uv;
#endif
    if (dy != 0) {
#ifndef AFFINE_TEXTURES
      myfloat izb = 1 / b.z;
      duv_z_ = (b.uv * izb - uv_z_) / dy;
#else
      duv_z_ = (b.uv - uv_z_) / dy;
#endif
    }
  }

  inline const Vector2 &uv_z() const {
    return uv_z_;
  }

  inline void Advance() {
    uv_z_ += duv_z_;
  }

  inline void Advance(const Integral value) {
    uv_z_ += duv_z_ * value;
  }

  inline Vector2 GetUV() const {
#ifndef AFFINE_TEXTURES
    return uv_z_ * this->z();
#else
    return uv_z_;
#endif
  }

 private:
  Vector2 uv_z_;
  Vector2 duv_z_;
};

struct RasterizerTraversableData : virtual public ZTraversableData
#ifdef PHONG_SHADING
    , public FullLightingData
#endif
{
 public:
#ifdef GOURAUD_SHADING
  Vector3 color;
#endif
};

#ifndef NO_SHADING

inline Vector3 PhongLight(LightingSourceData *int_sources, const Material &material,
                          const Vector3 &normal, const Vector3 &viewer,
                          const std::vector<LightSource> &sources) {
  Vector3 result;
  for (size_t d = 0; d < (size_t)result.rows(); ++d) {
    result(d) = 0;
    for (size_t i = 0; i < sources.size(); ++i) {
      result(d) += material.diffuse_color()(d) * int_sources[i].direction.dot(normal) * sources[i].diffuse;
      result(d) += material.specular_color()(d) * int_sources[i].reflection.dot(viewer) * sources[i].specular;
    }
  }
  return result;
}

#endif

inline sdlobj::Color VectorToColor(const Vector3 &vec) {
  int r = vec.x() * 0xFF, g = vec.y() * 0xFF, b = vec.z() * 0xFF;
  if (r > 0xFF) r = 0xFF;
  if (g > 0xFF) g = 0xFF;
  if (b > 0xFF) b = 0xFF;
  return Color(r, g, b);
}

template <class Integral = unsigned> class RasterizerTraversable : public Traversable<Integral>,
    public virtual ZTraversable<Integral>
#if defined(PHONG_SHADING)
    , public LightingTraversable
#elif defined(GOURAUD_SHADING)
    , public ColorTraversable
#endif
{
 public:
  struct TraversableContext {
   public:
    const Material *material;
    Surface *surface;
    ZBuffer *z_buffer;
    SurfacePainterWrapper *surface_painter;
#if defined(PHONG_SHADING) || defined(GOURAUD_SHADING)
    const vector<LightSource> *light_sources;
    myfloat ambient_light;
#else
    Uint32 pixel;
#endif
  };

  typedef RasterizerTraversableData DataType;
  typedef typename ZTraversable<Integral>::HorizontalTraversable ZTraversableHorisontal;
  typedef typename Traversable<Integral>::template HorizontalTraversable<TraversableContext> TraversableHorisontal;

  class HorizontalTraversable : public TraversableHorisontal, public virtual ZTraversableHorisontal
#ifdef PHONG_SHADING
      , public LightingTraversable::HorizontalTraversable
#endif
  {
   public:
    typedef TraversableContext ContextType;

    inline HorizontalTraversable(TraversableContext *context, const ScreenLine<Integral> &a, const RasterizerTraversable &da,
                                 const ScreenLine<Integral> &b, const RasterizerTraversable &db, const myfloat dx) :
      ZTraversableHorisontal(a, da, b, db, dx)
#if defined(PHONG_SHADING)
      , LightingTraversable::HorizontalTraversable(context, a, da, b, db, dx)
#elif defined(GOURAUD_SHADING)
      , ColorTraversable::HorizontalTraversable(context, a, da.color, b, db.color, dx)
#endif
    {
      z_buffer_ = context->z_buffer->Position(a.x(), a.y());
      painter_ = context->surface_painter->Position(a.x(), a.y());
    }

    inline bool Process(TraversableContext *context, const Integral x, const Integral y) {
      if (!z_buffer().Check(this->z())) return false;
#if defined(PHONG_SHADING) || defined(GOURAUD_SHADING)
#ifdef GOURAUD_SHADING
      Vector3 result = context->material->ambient_color() + color();
#else
      Vector3 result = context->material->ambient_color() +
          PhongLight(sources(), context->material, normal(), viewer(), context->light_sources);
#endif
      Uint32 pixel = context->surface->ColorToPixel(VectorToColor(result));
      painter().SetPixel(pixel);
#else
      painter().SetPixel(context->pixel);
#endif
      return true;
    }

    inline ZBuffer::Iterator z_buffer() {
      return z_buffer_;
    }

    inline SurfacePainterWrapper::Iterator painter() {
      return painter_;
    }

    inline void Advance() {
      ZTraversableHorisontal::Advance();
#if defined(PHONG_SHADING)
      LightingTraversable::HorizontalTraversable::Advance();
#elif defined(GOURAUD_SHADING)
      ColorTraversable::HorizontalTraversable::Advance();
#endif
      ++z_buffer_;
      ++painter_;
    }

    inline void Advance(const Integral value) {
      ZTraversableHorisontal::Advance(value);
#if defined(PHONG_SHADING)
      LightingTraversable::HorizontalTraversable::Advance(value);
#elif defined(GOURAUD_SHADING)
      ColorTraversable::HorizontalTraversable::Advance(value);
#endif
      z_buffer_ += value;
      painter_ += value;
    }

   private:
    ZBuffer::Iterator z_buffer_;
    SurfacePainterWrapper::Iterator painter_;
  };

  RasterizerTraversable(const ScreenLine<Integral> &line, const myfloat dy,
                        const DataType &a, const DataType &b) :
      ZTraversable<Integral>(line, dy, a.z, b.z)
#if defined(PHONG_SHADING)
    , LightingTraversable(line, a, b)
#elif defined(GOURAUD_SHADING)
    , ColorTraversable(line, a.color, b.color)
#endif
  {}

  inline void Advance() {
    ZTraversable<Integral>::Advance();
#if defined(PHONG_SHADING)
    LightingTraversable::Advance();
#elif defined(GOURAUD_SHADING)
    ColorTraversable::Advance();
#endif
  }

  inline void Advance(const Integral value) {
    ZTraversable<Integral>::Advance(value);
#if defined(PHONG_SHADING)
    LightingTraversable::Advance(value);
#elif defined(GOURAUD_SHADING)
    ColorTraversable::Advance(value);
#endif
  }
};

struct RasterizerTexturedTraversableData : public RasterizerTraversableData,
    public TextureTraversableData {
};

template <class Integral = unsigned> struct RasterizerTexturedTraversable : public RasterizerTraversable<Integral>,
    public TextureTraversable<Integral> {
 public:
  struct TraversableContext : public RasterizerTraversable<Integral>::TraversableContext {
    const SurfacePainterWrapper *texture;
#ifdef FLAT_SHADING
    Vector3 light;
#endif
  };

  typedef RasterizerTexturedTraversableData DataType;
  typedef typename TextureTraversable<Integral>::HorizontalTraversable TextureTraversableHorizontal;
  typedef typename RasterizerTraversable<Integral>::HorizontalTraversable RasterizerHorizontal;
  typedef typename ZTraversable<Integral>::HorizontalTraversable ZTraversableHorisontal;

  class HorizontalTraversable : public RasterizerHorizontal,
      public TextureTraversableHorizontal, virtual public ZTraversableHorisontal {
   public:
    typedef TraversableContext ContextType;

    inline HorizontalTraversable(TraversableContext *context, const ScreenLine<Integral> &a, const RasterizerTexturedTraversable &da,
                                 const ScreenLine<Integral> &b, const RasterizerTexturedTraversable &db, const myfloat dx) :
      ZTraversableHorisontal(a, da, b, db, dx),
      RasterizerHorizontal(context, a, da, b, db, dx),
      TextureTraversableHorizontal(nullptr, a, da, b, db, dx) {}

    inline bool Process(TraversableContext *context, const Integral x, const Integral y) {
      if (!this->z_buffer().Check(this->z())) return false;
      Vector2 uv = this->GetUV();
      int uvx = uv.x(),
          uvy = uv.y();
//#ifndef AFFINE_TEXTURES
      if (uvx < 0)
        uvx = 0;
      if (uvy < 0)
        uvy = 0;
      int w = context->texture->surface()->width() - 1,
          h = context->texture->surface()->height() - 1;
      if (uvx > w)
        uvx = w;
      if (uvy > h)
        uvy = h;
//#endif
      Uint32 pixel = context->texture->GetPixel(uvx, uvy);
#ifndef NO_SHADING
      sdlobj::Color tcolor = context->material->texture()->PixelToColor(pixel);
      Vector3 result(tcolor.r, tcolor.g, tcolor.b);
      result = Componentwise(context->material->ambient_color().transpose(), result);
#if defined(GOURAUD_SHADING)
      result += color();
#elif defined(FLAT_SHADING)
      result += context->light;
#else
      result += PhongLight(sources(), context->material, normal(), viewer(), context->light_sources);
#endif
      pixel = context->surface->ColorToPixel(VectorToColor(result));
#endif

      this->painter().SetPixel(pixel);
      return true;
    }

    inline void Advance() {
      RasterizerHorizontal::Advance();
      TextureTraversableHorizontal::Advance();
    }

    inline void Advance(const Integral value) {
      RasterizerHorizontal::Advance(value);
      TextureTraversableHorizontal::Advance(value);
    }
  };

  inline RasterizerTexturedTraversable(const ScreenLine<Integral> &line, const myfloat dy,
                                       const DataType &a, const DataType &b) :
      ZTraversable<Integral>(line, dy, a.z, b.z), RasterizerTraversable<Integral>(line, dy, a, b),
      TextureTraversable<Integral>(line, dy, a, b) {}

  inline void Advance() {
    RasterizerTraversable<Integral>::Advance();
    TextureTraversable<Integral>::Advance();
  }

  inline void Advance(const Integral value) {
    RasterizerTraversable<Integral>::Advance(value);
    TextureTraversable<Integral>::Advance(value);
  }
};

template <bool kTextures> void DrawTriangle(const size_t tri_i, const TransformedObject &cacheobject,
        const SceneObject &object, RasterizerTexturedTraversable<unsigned>::TraversableContext *context) {
  size_t triangles_size = 1;
  size_t points_size = IndexedTriangle::kPointsSize;

  IndexedTriangle triangles_buf[kPolygonsSize];
  Vector3 points_buf[kPointsSize];

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

  RasterizerTexturedTraversableData data[points_size];
  for (size_t i = 0; i < points_size; ++i) {
    data[i].z = points_buf[i].z();
  }

#if defined(PHONG_SHADING) || defined(GOURAUD_SHADING)

  for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
    const int pt = triangles_buf[0].points[i];
#if defined(PHONG_SHADING)
    data[i].normal = object.positioned_vertex_normals()[pt];
    data[i].viewer = cacheobject.lighting_data[pt].viewer;
    data[i].sources_data = cacheobject.lighting_data[pt].sources_data;
#elif defined(GOURAUD_SHADING)
    data[i].color = PhongLight(cacheobject.lighting_data[pt].sources_data.data(), *context.material,
                               object.positioned_vertex_normals()[pt], cacheobject.lighting_data[pt].viewer,
                               context.ligting_sources);
#endif
  }
  bool normal_found[points_size];
  if (points_size > 3) {
#if defined(PHONG_SHADING)
    TriangleTraverser<LightingTraversable<myfloat>> traverser(points_buf[0], data[0],
                                                    points_buf[1], data[1],
                                                    points_buf[2], data[2]);
#elif defined(GOURAUD_SHADING)
    TriangleTraverser<ColorTraversable<myfloat>> traverser(points_buf[0], data[0].color,
                                                    points_buf[1], data[1].color,
                                                    points_buf[2], data[2].color);
#endif
    memset((bool *)&normal_found, true, IndexedTriangle::kPointsSize * sizeof(bool));
    memset((bool *)&normal_found + IndexedTriangle::kPointsSize,
           false, (points_size - IndexedTriangle::kPointsSize) * sizeof(bool));
    for (size_t i = 0; i < triangles_size; ++i) {
      IndexedTriangle &pol = triangles_buf[i];
      if (pol.points[0] == -1) continue;
      for (size_t j = 0; j < IndexedTriangle::kPointsSize; ++j) {
        size_t pt = pol.points[j];
        if (!normal_found[pt]) {
#if defined(PHONG_SHADING)
          Assert(traverser.Point(&data[pt], points_buf[pt].x, points_buf[pt].y));
#elif defined(GOURAUD_SHADING)
          Assert(traverser.Point(&data[pt].color, points_buf[pt].x, points_buf[pt].y));
#endif
          normal_found[pt] = true;
        }
      }
    }
  }

#endif

  if (kTextures) {
    for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
      data[i].uv = (*object.model()->uv_coords())[tri.points[i]];
      data[i].uv.x() *= context->texture->surface()->width() - 1;
      data[i].uv.y() *= context->texture->surface()->height() - 1;
    }
    if (points_size > 3) {
      TriangleTraverser<TextureTraversable<myfloat>> traverser(points_buf[0], data[0],
                                                       points_buf[1], data[1],
                                                       points_buf[2], data[2]);
#if defined(PHONG_SHADING) || defined(GOURAUD_SHADING)
      for (size_t i = IndexedTriangle::kPointsSize; i < points_size; ++i) {
        if (normal_found[i]) {
          Assert(traverser.Point(&data[i].uv, points_buf[i].x, points_buf[i].y));
        }
      }
#else
      bool normal_found[points_size];
      /*std::fill_n((bool *)&normal_found, IndexedTriangle::kPointsSize, true);
      std::fill_n((bool *)&normal_found + IndexedTriangle::kPointsSize,
                  points_size - IndexedTriangle::kPointsSize, true);*/
      memset((bool *)&normal_found, true, IndexedTriangle::kPointsSize * sizeof(bool));
      memset((bool *)&normal_found + IndexedTriangle::kPointsSize,
             false, (points_size - IndexedTriangle::kPointsSize) * sizeof(bool));
      for (size_t i = 0; i < triangles_size; ++i) {
        IndexedTriangle &pol = triangles_buf[i];
        if (pol.points[0] == -1) continue;
        for (size_t j = 0; j < IndexedTriangle::kPointsSize; ++j) {
          size_t pt = pol.points[j];
          if (!normal_found[pt]) {
            Assert(traverser.Point<true>(&data[pt].uv, points_buf[pt].x(), points_buf[pt].y()));
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

static const myfloat system_transform_r[] = {0, 0, 1, 0,
                                             -1, 0, 0, 0,
                                             0, -1, 0, 0,
                                             0, 0, 0, 0};

static const AffineTransform system_transform = AffineTransform(Matrix4(system_transform_r));

void Rasterizer::PreRenderStep() {
  AffineTransform transform = system_transform * context()->camera.GetMatrixTo();
#ifndef NO_NORMAL_FACE_CLIPPING
  Vector3 normal = context()->camera.GetRotateMatrixFrom() * Position::kCameraDirection;
#endif

  TransformedObjectMap new_cache;

#if defined(PHONG_SHADING) || defined(GOURAUD_SHADING)
  Vector3 camera = context()->camera.GetVector3();
  Scene::LightSourceVector &light_sources = context()->scene.light_sources();
#endif

  for (auto &object : context()->scene.objects()) {
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

    const Vector3Vector &srcpoints = object->positioned_points();

    hit->second->material_overlay.reset();
    int o_size = object->positioned_points().size();
    Vector3Vector &points = hit->second->points;
    std::vector<bool> &point_flags = hit->second->point_flags;
    std::vector<size_t> &triangle_indexes = hit->second->triangle_indexes;

#if defined(PHONG_SHADING) || defined(GOURAUD_SHADING)
    const Vector3 &srcnormals = object->positioned_vertex_normals();

    std::vector<LightingData> &lighting_data = hit->second->lighting_data;
    lighting_data.resize(o_size);
    if (lighting_data[0].sources_data.size() < light_sources.size()) {
      for (auto &i : lighting_data) {
        i.sources_data = SizedRuntimeArray(light_sources.size());
      }
    }
#endif

    points.resize(o_size);
    point_flags.assign(o_size, false);
    triangle_indexes.clear();
    triangle_indexes.reserve(model->polygons().size() / 2);

    auto p_i = model->polygons().begin();
    auto n_i = object->positioned_polygon_normals().begin();
    size_t i = 0;
    for (auto pe = model->polygons().end(); p_i != pe; ++p_i, ++n_i, ++i) {
#ifndef NO_NORMAL_FACE_CLIPPING
      if (normal.dot(*n_i) <= 0) {
#endif
        triangle_indexes.push_back(i);
        const int * const &indexes = p_i->points;
        for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
          const int &index = indexes[i];
          if (!point_flags[index]) {
            const Vector3 &src = srcpoints[index];
            Vector3 dest = transform * src;
            context()->camera.PerspectiveTransform(dest);
            points[index] = move(dest);
#if defined(PHONG_SHADING) || defined(GOURAUD_SHADING)
            LightingData &lighting = lighting_data[index];
            lighting.viewer = camera - src;
            for (size_t i = 0; i < light_sources.size(); ++i) {
              lighting.sources_data[i].direction = light_sources[i].position - src;
              lighting.sources_data[i].reflection = Reflection(lighting.sources_data[i].direction,
                                                               srcnormals[i]);
            }
#endif
            point_flags[index] = true;
          }
        }
#ifndef NO_NORMAL_FACE_CLIPPING
      }
#endif
    }
  }

  *cache_ = std::move(new_cache);
}

#ifndef NO_DRAW_SURFACE
void DrawSurface(RasterizerTexturedTraversable<>::TraversableContext &context, const Camera &camera) {
  AffineTransform transform = system_transform * RotateTransform(-camera.pitch, Vector3::UnitY());

  Vector3 far(camera.view_limit(), 0, -camera.z);
  far = transform * far;
  //camera.PerspectiveTransform(far);
  far.x() += camera.width() / 2.0;
  far.y() += camera.height() / 2.0;
  Vector3 near(-camera.view_limit(), 0, -camera.z);
  near = transform * near;
  //camera.PerspectiveTransform(near);
  near.x() += camera.width() / 2.0;
  near.y() += camera.height() / 2.0;
  if (far.y() > near.y()) std::swap(far, near);
  if (far.y() < context.surface->height() && near.y() >= 0) {
    if (!Clip<ZClipper<Less<myfloat>>>(far, near, 0)) return;
    if (!Clip<YClipper<Less<myfloat>>>(far, near, 0)) return;
    if (!Clip<YClipper<Greater<myfloat>>>(far, near, context.surface->height() - 1)) return;

    int y = far.y(), fy = near.y();
    int dy = fy - y;
    context.surface->FillRect(0, y, context.surface->width(), dy, 0x5F5F5F);
    myfloat z = far.z();
    myfloat dz = 0;
    if (y != fy) dz = (near.z() - z) / dy;
    for (int cy = y; cy < fy; ++cy) {
      std::fill_n(context.z_buffer->Position(0, cy), context.surface->width(), z);
      z += dz;
    }
    std::fill_n(context.z_buffer->Position(0, fy), context.surface->width(), z);
  }
}
#endif

inline void PrepareAndDraw(RasterizerTexturedTraversable<>::TraversableContext &tcontext,
                    const Material &material, SurfacePainterWrapper *&texture_painter,
                    Surface *&texture, const size_t t, const Surface &surface,
                    const TransformedObject &transformed, const SceneObject &object
#ifdef FLAT_SHADING
                    , const Context &ccontext
#endif
                    ) {
  tcontext.material = &material;
#ifdef FLAT_SHADING
  // getting camera vector each triangle
  Vector3 viewer = ccontext.camera.GetVector3() - object.positioned_centers()[t];
  const std::vector<LightSource> &light_sources = ccontext.scene.light_sources();
  const Vector3 &vertex_normal = object.positioned_polygon_normals()[t];
  RuntimeArray<LightingSourceData> sources(light_sources.size());
  for (size_t i = 0; i < ccontext.scene.light_sources().size(); ++i) {
    sources[i].direction = light_sources[i].position - object.positioned_centers()[t];
    sources[i].reflection = Reflection(sources[i].direction,
                                       vertex_normal);
  }
  if (material.texture()) {
    tcontext.light = PhongLight(sources.data(), material, vertex_normal, viewer, light_sources);
  } else {
    Vector3 color = material.ambient_color() +
        PhongLight(sources.data(), material, vertex_normal, viewer, light_sources);
    tcontext.pixel = surface.ColorToPixel(VectorToColor(color));
  }
#endif
  if (material.texture()) {
    if (texture != material.texture().get()) {
      if (texture_painter) delete texture_painter;
      texture = material.texture().get();
#if defined(NO_SHADING) || defined(COMPILE_TIME_BPP)
      if (!texture->DisplayFormat()) {
        *texture = texture->DisplayConvert();
      }
#endif
      texture_painter = GetSurfacePainter(texture);
      texture_painter->StartDrawing();
    }
    tcontext.texture = texture_painter;
    DrawTriangle<true>(t, transformed, object, &tcontext);
  } else {
#ifdef NO_SHADING
    tcontext.pixel = surface.ColorToPixel(VectorToColor(material.ambient_color()));
#endif
    DrawTriangle<false>(t, transformed, object, &tcontext);
  }
}

void Rasterizer::Paint(sdlobj::Surface &surface) {
  z_buffer_.set_size(surface.width(), surface.height());
  z_buffer_.Clear();

  SurfacePainterWrapper *surface_painter = GetSurfacePainter(&surface);

  RasterizerTexturedTraversable<>::TraversableContext tcontext;
  tcontext.surface = &surface;
  tcontext.z_buffer = &z_buffer_;
  tcontext.surface_painter = surface_painter;
#if defined(GOURAUD_SHADING) || defined(PHONG_SHADING)
  tcontext.light_sources = context()->scene.light_sources();
#endif

#ifndef NO_DRAW_SURFACE
  DrawSurface(tcontext, context()->camera);
#endif

  SurfacePainterWrapper *texture_painter = nullptr;
  Surface *texture = nullptr;

  surface_painter->StartDrawing();
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
        PrepareAndDraw(tcontext, *material, texture_painter, texture, t,
                       surface, transformed, *object
#ifdef FLAT_SHADING
                       , *context()
#endif
                       );
      }
    } else {
      for (const auto &t : transformed.triangle_indexes) {
        const Material *material = object->model()->materials()[t].get();
        PrepareAndDraw(tcontext, *material, texture_painter, texture, t,
                       surface, transformed, *object
#ifdef FLAT_SHADING
                       , *context()
#endif
                       );
      }
    }
  }

  if (texture_painter) delete texture_painter;
  delete surface_painter;
}

void Rasterizer::Clear() {
  cache_->clear();
}
