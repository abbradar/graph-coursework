#include <algorithm>
#include "common/math.h"
#include "common/debug.h"
#include "common/exception.h"
#include "surfacepainterfactory.h"
#include "triangletraverser.h"
#include "config.h"
#include "colortransforms.h"
#include "rasterizer.h"

using namespace std;
using namespace sdlobj;

Rasterizer::Rasterizer(const std::shared_ptr<Context> &context) :
   ContextUser(context), cache_(new TransformedObjectMap()), z_buffer_(0, 0),
   surface_color_(0x5F, 0x5F, 0x5F)
#ifndef NO_SHADING
 , light_sources_(new std::vector<FullLightSource>)
#endif
{
  context->transformed_objects = cache_;
  context->light_sources = light_sources_;
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

template <class Integral = myfloat> class LightDataTraversable : virtual public Traversable<Integral> {
 public:
  typedef FullLightingData DataType;

  class HorizontalTraversable : virtual public Traversable<Integral>::template HorizontalTraversable<DataType> {
   public:
    inline HorizontalTraversable(FullLightingData *, const ScreenLine<Integral> &la, const LightDataTraversable<Integral> &a,
                                 const ScreenLine<Integral> &lb, const LightDataTraversable<Integral> &b,
                                 const myfloat dx) :
        normal_(a.normal()), viewer_(a.viewer()), sources_size_(a.sources_size())
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
        , sources_(sources_size_)
#endif
    {
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
      memcpy(sources_.data(), a.sources().data(), sources_size_ * sizeof(LightingSourceData));
#else
      sources_.Assign(a.sources(), sources_size_);
#endif
      if (dx != 0) {
        dnormal_ = (b.normal() - normal_) / dx;
        dviewer_ = (b.viewer() - viewer_) / dx;
        dsources_ = RuntimeArray<LightingSourceData>(sources_size_);
        for (size_t i = 0; i < sources_size_; ++i) {
          dsources_[i].direction = (b.sources()[i].direction - sources_[i].direction) / dx;
          dsources_[i].reflection = (b.sources()[i].reflection - sources_[i].reflection) / dx;
        }
      } else {
        dsources_ = nullptr;
      }
    }

    inline HorizontalTraversable(const HorizontalTraversable &other) :
      normal_(other.normal_), dnormal_(other.dnormal_), viewer_(other.viewer_),
      dviewer_(other.dviewer_), sources_size_(other.sources_size_)
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
        , sources_(sources_size_)
#endif
    {
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
      memcpy(sources_.data(), other.sources_.data(), sources_size_ * sizeof(LightingSourceData));
#else
      sources_.Assign(other.sources_, sources_size_);
#endif
      if (other.dsources_.data()) {
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
        dsources_ = RuntimeArray<LightingSourceData>(sources_size_);
        memcpy(dsources_.data(), other.dsources_.data(), sources_size_ * sizeof(LightingSourceData));
#else
        dsources_.Assign(other.dsources_, sources_size_);
#endif
      }
    }

    inline HorizontalTraversable(HorizontalTraversable &&other) :
      normal_(other.normal_), dnormal_(other.dnormal_), viewer_(other.viewer_),
      dviewer_(other.dviewer_), sources_size_(other.sources_size_),
      sources_(std::forward<RuntimeArray<LightingSourceData>>(other.sources_)),
      dsources_(std::forward<RuntimeArray<LightingSourceData>>(other.dsources_)) {}

    HorizontalTraversable &operator =(const HorizontalTraversable &other) {
      normal_ = other.normal_;
      viewer_ = other.viewer_;
      sources_size_ = other.sources_size_;
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
      sources_ = RuntimeArray<LightingSourceData>(sources_size_);
      memcpy(sources_.data(), other.sources_.data(), sources_size_ * sizeof(LightingSourceData));
#else
      sources_.Assign(other.sources_, sources_size_);
#endif
      if (other.dsources_.data()) {
        dnormal_ = other.dnormal_;
        dviewer_ = other.dviewer_;
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
        dsources_ = RuntimeArray<LightingSourceData>(sources_size_);
        memcpy(dsources_.data(), other.dsources_.data(), sources_size_ * sizeof(LightingSourceData));
#else
        dsources_.Assign(other.dsources_, sources_size_);
#endif
      }
      return *this;
    }

    inline HorizontalTraversable &operator =(HorizontalTraversable &&other) {
      normal_ = other.normal_;
      viewer_ = other.viewer_;
      dnormal_ = other.dnormal_;
      dviewer_ = other.dviewer_;
      sources_size_ = other.sources_size_;
      sources_ = std::move(other.sources_);
      dsources_ = std::move(other.dsources_);
      return *this;
    }

    // this should run only once, so - some dirty "hack"
    inline bool Process(FullLightingData *context, const Integral, const Integral) {
      context->viewer = viewer_;
      context->normal = normal_;
      context->sources_data.Assign(std::move(sources_), sources_size_);
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

    inline RuntimeArray<LightingSourceData> &sources() {
      return sources_;
    }

    inline const RuntimeArray<LightingSourceData> &sources() const {
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

  inline LightingTraversable() = default;

  inline LightDataTraversable(const ScreenLine<Integral> &line, const myfloat dy, const DataType &a, const DataType &b) :
      normal_(a.normal), viewer_(a.viewer), sources_size_(a.sources_data.size())
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
      , sources_(sources_size_)
#endif
  {
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
    memcpy(sources_.data(), a.sources_data.data(), sources_size_ * sizeof(LightingSourceData));
#else
    sources_.Assign(a.sources_data.data(), sources_size_);
#endif
    if (dy != 0) {
      dnormal_ = (b.normal - normal_) / dy;
      dviewer_ = (b.viewer - viewer_) / dy;
      dsources_ = RuntimeArray<LightingSourceData>(sources_size_);
      for (size_t i = 0; i < sources_size_; ++i) {
        dsources_[i].direction = (b.sources_data[i].direction - sources_[i].direction) / dy;
        dsources_[i].reflection = (b.sources_data[i].reflection - sources_[i].reflection) / dy;
      }
    }
  }

  inline LightDataTraversable(LightDataTraversable &&other) :
    normal_(other.normal_), dnormal_(other.dnormal_), viewer_(other.viewer_),
    dviewer_(other.dviewer_), sources_size_(other.sources_size_),
    sources_(std::forward<RuntimeArray<LightingSourceData>>(other.sources_)),
    dsources_(std::forward<RuntimeArray<LightingSourceData>>(other.dsources_)) {}

  inline LightDataTraversable(const LightDataTraversable &other) :
      normal_(other.normal_), dnormal_(other.dnormal_), viewer_(other.viewer_),
      dviewer_(other.dviewer_), sources_size_(other.sources_size_)
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
      , sources_(sources_size_)
#endif
  {
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
    memcpy(sources_.data(), other.sources_.data(), sources_size_ * sizeof(LightingSourceData));
#else
    sources_.Assign(other.sources_, sources_size_);
#endif
    if (other.dsources_.data()) {
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
      dsources_ = RuntimeArray<LightingSourceData>(sources_size_);
      memcpy(dsources_.data(), other.dsources_.data(), sources_size_ * sizeof(LightingSourceData));
#else
      dsources_.Assign(other.dsources_, sources_size_);
#endif
    }
  }

  inline LightDataTraversable &operator =(LightDataTraversable &&other) {
    normal_ = other.normal_;
    viewer_ = other.viewer_;
    dnormal_ = other.dnormal_;
    dviewer_ = other.dviewer_;
    sources_size_ = other.sources_size_;
    sources_ = std::move(other.sources_);
    dsources_ = std::move(other.dsources_);
    return *this;
  }

  LightDataTraversable &operator =(const LightDataTraversable &other) {
    normal_ = other.normal_;
    viewer_ = other.viewer_;
    sources_size_ = other.sources_size_;
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
    sources_ = RuntimeArray<LightingSourceData>(sources_size_);
    memcpy(sources_.data(), other.sources_.data(), sources_size_ * sizeof(LightingSourceData));
#else
    sources_.Assign(other.sources_, sources_size_);
#endif
    if (other.dsources_.data()) {
      dnormal_ = other.dnormal_;
      dviewer_ = other.dviewer_;
#ifdef LIGHTSOURCES_UNSAFE_MEMCPY
      dsources_ = RuntimeArray<LightingSourceData>(sources_size_);
      memcpy(dsources_.data(), other.dsources_.data(), sources_size_ * sizeof(LightingSourceData));
#else
      dsources_.Assign(other.dsources_, sources_size_);
#endif
    }
    return *this;
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

  inline RuntimeArray<LightingSourceData> &sources() {
    return sources_;
  }

  inline const RuntimeArray<LightingSourceData> &sources() const {
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

template <class Integral = myfloat> class LightTraversable : virtual public Traversable<Integral> {
 public:
  typedef Vector3 DataType;

  class HorizontalTraversable : virtual public Traversable<Integral>::template HorizontalTraversable<DataType> {
   public:
    inline HorizontalTraversable(Vector3 *, const ScreenLine<Integral> &la, const LightTraversable<Integral> &a,
                                 const ScreenLine<Integral> &lb, const LightTraversable<Integral> &b,
                                 const myfloat dx) : light_(a.light())
    {
      if (dx != 0) {
        dlight_ = (b.light() - light_) / dx;
      }
    }

    inline bool Process(DataType *context, const Integral, const Integral) {
      *context = light_;
      return true;
    }

    inline void Advance() {
      light_ += dlight_;
    }

    inline void Advance(const Integral value) {
      light_ += dlight_ * value;
    }

    inline const Vector3 &light() const {
      return light_;
    }

   private:
    Vector3 light_;
    Vector3 dlight_;
  };

  inline LightTraversable() = default;

  inline LightTraversable(const ScreenLine<Integral> &line, const myfloat dy, const DataType &a, const DataType &b) :
      light_(a) {
    if (dy != 0) {
      dlight_ = (b - a) / dy;
    }
  }

  inline void Advance() {
    light_ += dlight_;
  }

  inline void Advance(const Integral value) {
    light_ += dlight_ * value;
  }

  inline const Vector3 &light() const {
    return light_;
  }

 private:
  Vector3 light_;
  Vector3 dlight_;
};

#endif

struct ZTraversableData {
  myfloat z;
};

struct UVTraversableData
#ifndef AFFINE_TEXTURES
    : virtual public ZTraversableData
#endif
{
  Vector2 uv;
};

template <class Integral = myfloat> class UVTraversable : virtual public Traversable<Integral>
#ifndef AFFINE_TEXTURES
    , virtual public ZTraversable<Integral>
#endif
{
 public:
  typedef UVTraversableData DataType;
  typedef typename Traversable<Integral>::template HorizontalTraversable<Vector2> TraversableHorizontal;
#ifndef AFFINE_TEXTURES
  typedef typename ZTraversable<Integral>::HorizontalTraversable ZTraversableHorizontal;
#endif

  class HorizontalTraversable : public TraversableHorizontal
#ifndef AFFINE_TEXTURES
      , public virtual ZTraversableHorizontal
#endif
  {
   public:
    inline HorizontalTraversable(Vector2 *, const ScreenLine<Integral> &la, const UVTraversable &a,
                                 const ScreenLine<Integral> &lb, const UVTraversable &b, const myfloat dx) :
#ifndef AFFINE_TEXTURES
      ZTraversableHorizontal(la, a, lb, b, dx),
#endif
      uv_z_(a.uv_z())
    {
      if (dx != 0) {
        duv_z_ = (b.uv_z() - uv_z_) / dx;
      }
    }

    inline const Vector2 &uv_z() const {
      return uv_z_;
    }

    inline void Advance() {
#ifndef AFFINE_TEXTURES
      ZTraversableHorizontal::Advance();
#endif
      AdvanceUV();
    }

    inline void AdvanceUV() {
      uv_z_ += duv_z_;
    }

    inline void Advance(const Integral value) {
#ifndef AFFINE_TEXTURES
      ZTraversableHorizontal::Advance(value);
 #endif
      AdvanceUV(value);
    }

    inline void AdvanceUV(const Integral value) {
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

  inline UVTraversable() = default;

  inline UVTraversable(const ScreenLine<Integral> &line, const myfloat dy,
                            const DataType &a, const DataType &b)
#ifndef AFFINE_TEXTURES
      : ZTraversable<Integral>(line, dy, a.z, b.z)
#endif
  {
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
#ifndef AFFINE_TEXTURES
    ZTraversable<Integral>::Advance();
#endif
    AdvanceUV();
  }

  inline void AdvanceUV() {
    uv_z_ += duv_z_;
  }

  inline void Advance(const Integral value) {
#ifndef AFFINE_TEXTURES
    ZTraversable<Integral>::Advance(value);
#endif
    AdvanceUV(value);
  }

  inline void AdvanceUV(const Integral value) {
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

#ifndef NO_SHADING

inline Vector3 PhongLight(const LightingSourceData *int_sources, const Material &material,
                          const Vector3 &normal, const Vector3 &viewer,
                          const std::vector<FullLightSource> &sources) {
  Vector3 result;
  for (size_t d = 0; d < (size_t)result.rows(); ++d) {
    result(d) = 0;
    for (size_t i = 0; i < sources.size(); ++i) {
      myfloat diffuse_d = int_sources[i].direction.dot(normal);
      if (diffuse_d > 0)
        result(d) += material.diffuse_color()(d) * diffuse_d * sources[i].diffuse;
      myfloat specular_d = int_sources[i].reflection.dot(viewer);
      if (specular_d > 0)
        result(d) += material.specular_color()(d) * pow(specular_d, material.shininess()) * sources[i].specular;
    }
  }
  return result;
}

#endif

struct RasterizerData : virtual public ZTraversableData {};

template <class Integral = unsigned> class RasterizerTraversable :
    public virtual ZTraversable<Integral>, public Traversable<Integral> {
 public:
  struct TraversableContext {
   public:
    const Material *material;
    Surface *surface;
    ZBuffer *z_buffer;
    SurfacePainterWrapper *surface_painter;
    Uint32 pixel;
  };

  typedef RasterizerData DataType;

  typedef typename ZTraversable<Integral>::HorizontalTraversable ZTraversableHorisontal;
  typedef typename Traversable<Integral>::template HorizontalTraversable<TraversableContext> TraversableHorisontal;

  class HorizontalTraversable : public TraversableHorisontal, public virtual ZTraversableHorisontal {
   public:
    typedef TraversableContext ContextType;

    inline HorizontalTraversable(TraversableContext *context, const ScreenLine<Integral> &a, const RasterizerTraversable &da,
                                 const ScreenLine<Integral> &b, const RasterizerTraversable &db, const myfloat dx) :
        ZTraversableHorisontal(a, da, b, db, dx) {
      z_buffer_ = context->z_buffer->Position(a.x(), a.y());
      painter_ = context->surface_painter->Position(a.x(), a.y());
    }

    inline bool Process(TraversableContext *context, const Integral x, const Integral y) {
      if (!z_buffer().Check(this->z())) return false;
      painter().SetPixel(context->pixel);
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
      ++z_buffer_;
      ++painter_;
    }

    inline void Advance(const Integral value) {
      ZTraversableHorisontal::Advance(value);
      z_buffer_ += value;
      painter_ += value;
    }

   private:
    ZBuffer::Iterator z_buffer_;
    SurfacePainterWrapper::Iterator painter_;
  };

  RasterizerTraversable(const ScreenLine<Integral> &line, const myfloat dy,
                        const DataType &a, const DataType &b) :
      ZTraversable<Integral>(line, dy, a.z, b.z) {}

  inline void Advance() {
    ZTraversable<Integral>::Advance();
  }

  inline void Advance(const Integral value) {
    ZTraversable<Integral>::Advance(value);
  }
};

#ifndef NO_SHADING

struct RasterizerLightingData : virtual public RasterizerData
#ifdef PHONG_SHADING
    , public FullLightingData
#endif
{
 public:
#if defined(GOURAUD_SHADING) || defined(FLAT_SHADING)
  Vector3 light;
#endif
};

template <class Integral = unsigned> class RasterizerLightingTraversable :
    public virtual RasterizerTraversable<Integral>
#if defined(PHONG_SHADING)
    , public LightDataTraversable<Integral>
#elif defined(GOURAUD_SHADING)
    , public LightTraversable<Integral>
#endif
{
 public:
  struct TraversableContext : virtual public RasterizerTraversable<Integral>::TraversableContext {
   public:
    Vector3 ambient;
#if defined(GOURAUD_SHADING) || defined(FLAT_SHADING)
    const vector<FullLightSource> *light_sources;
#else
    Vector3 light;
#endif
  };

  typedef RasterizerLightingData DataType;

  typedef typename RasterizerTraversable<Integral>::HorizontalTraversable
    RasterizerTraversableHorisontal;
  typedef typename ZTraversable<Integral>::HorizontalTraversable
    ZHorisontal;
#if defined(PHONG_SHADING)
  typedef typename LightDataTraversable<Integral>::HorizontalTraversable LightingHorizontal;
#elif defined(GOURAUD_SHADING)
  typedef typename LightTraversable<Integral>::HorizontalTraversable LightHorizontal;
#endif

  class HorizontalTraversable : public virtual RasterizerTraversableHorisontal
#ifdef PHONG_SHADING
      , public LightingHorizontal
#elif defined(GOURAUD_SHADING)
      , public LightHorizontal
#endif
  {
   public:
    typedef TraversableContext ContextType;

    inline HorizontalTraversable(TraversableContext *context, const ScreenLine<Integral> &a, const RasterizerLightingTraversable &da,
                                 const ScreenLine<Integral> &b, const RasterizerLightingTraversable &db, const myfloat dx) :
      ZHorisontal(a, da, b, db, dx),
      RasterizerTraversableHorisontal(context, a, da, b, db, dx)
#if defined(PHONG_SHADING)
      , LightingHorizontal(nullptr, a, da, b, db, dx)
#elif defined(GOURAUD_SHADING)
      , LightHorizontal(nullptr, a, da, b, db, dx)
#endif
    {}

    inline bool Process(TraversableContext *context, const Integral x, const Integral y) {
      if (!this->z_buffer().Check(this->z())) return false;
#ifdef GOURAUD_SHADING
      Vector3 result = (context->ambient + this->light()) * 0xFF;
#else
      Vector3 result = (context->ambient +
          PhongLight(this->sources().data(), *context->material, this->normal(),
                     this->viewer(), *context->light_sources)) * 0xFF;
#endif
      Uint32 pixel = context->surface->ColorToPixel(VectorToColor(result));
      this->painter().SetPixel(pixel);
      return true;
    }

    inline void Advance() {
      RasterizerTraversableHorisontal::Advance();
      AdvanceLight();
    }

    inline void AdvanceLight() {
#if defined(PHONG_SHADING)
      LightingHorizontal::Advance();
#elif defined(GOURAUD_SHADING)
      LightHorizontal::Advance();
#endif
    }

    inline void Advance(const Integral value) {
      RasterizerTraversableHorisontal::Advance(value);
      AdvanceLight(value);
    }

    inline void AdvanceLight(const Integral value) {
#if defined(PHONG_SHADING)
      LightingHorizontal::Advance(value);
#elif defined(GOURAUD_SHADING)
      LightHorizontal::Advance(value);
#endif
    }
  };

  RasterizerLightingTraversable(const ScreenLine<Integral> &line, const myfloat dy,
                        const DataType &a, const DataType &b) :
      ZTraversable<Integral>(line, dy, a.z, b.z),
      RasterizerTraversable<Integral>(line, dy, a, b)
#if defined(PHONG_SHADING)
    , LightDataTraversable<Integral>(line, dy, a, b)
#elif defined(GOURAUD_SHADING)
    , LightTraversable<Integral>(line, dy, a.light, b.light)
#endif
  {}

  inline void Advance() {
    RasterizerTraversable<Integral>::Advance();
    AdvanceLight();
  }

  inline void AdvanceLight() {
#if defined(PHONG_SHADING)
    LightDataTraversable<Integral>::Advance();
#elif defined(GOURAUD_SHADING)
    LightTraversable<Integral>::Advance();
#endif
  }

  inline void Advance(const Integral value) {
    RasterizerTraversable<Integral>::Advance(value);
    AdvanceLight(value);
  }

  inline void AdvanceLight(const Integral value) {
#if defined(PHONG_SHADING)
    LightDataTraversable<Integral>::Advance(value);
#elif defined(GOURAUD_SHADING)
    LightTraversable<Integral>::Advance(value);
#endif
  }
};

#endif

struct RasterizerTexturedData : virtual public RasterizerData,
    public UVTraversableData {};

template <class Integral = unsigned> struct RasterizerTexturedTraversable : public virtual
    RasterizerTraversable<Integral>,
    public UVTraversable<Integral> {
 public:
  struct TraversableContext : virtual public RasterizerTraversable<Integral>::TraversableContext {
    const SurfacePainterWrapper *texture;
  };

  typedef RasterizerTexturedData DataType;

  typedef typename UVTraversable<Integral>::HorizontalTraversable UVTraversableHorizontal;
  typedef typename RasterizerTraversable<Integral>::HorizontalTraversable RasterizerHorizontal;
  typedef typename ZTraversable<Integral>::HorizontalTraversable ZHorizontal;

  class HorizontalTraversable : public virtual RasterizerHorizontal,
      public UVTraversableHorizontal {
   public:
    typedef TraversableContext ContextType;

    inline HorizontalTraversable(TraversableContext *context, const ScreenLine<Integral> &a, const RasterizerTexturedTraversable &da,
                                 const ScreenLine<Integral> &b, const RasterizerTexturedTraversable &db, const myfloat dx) :
      ZHorizontal(a, da, b, db, dx),
      RasterizerHorizontal(context, a, da, b, db, dx),
      UVTraversableHorizontal(nullptr, a, da, b, db, dx) {}

    inline bool Process(TraversableContext *context, const Integral x, const Integral y) {
      if (!this->z_buffer().Check(this->z())) return false;
      this->painter().SetPixel(GetPixel(context));
      return true;
    }

    inline Uint32 GetPixel(const TraversableContext *context) {
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
      return context->texture->GetPixel(uvx, uvy);
    }

    inline void Advance() {
      RasterizerHorizontal::Advance();
      AdvanceTexture();
    }

    inline void AdvanceTexture() {
      UVTraversableHorizontal::AdvanceUV();
    }

    inline void Advance(const Integral value) {
      RasterizerHorizontal::Advance(value);
      AdvanceTexture(value);
    }

    inline void AdvanceTexture(const Integral value) {
      UVTraversableHorizontal::AdvanceUV(value);
    }
  };

  inline RasterizerTexturedTraversable(const ScreenLine<Integral> &line, const myfloat dy,
                                       const DataType &a, const DataType &b) :
      ZTraversable<Integral>(line, dy, a.z, b.z),
      RasterizerTraversable<Integral>(line, dy, a, b),
      UVTraversable<Integral>(line, dy, a, b) {}

  inline void Advance() {
    RasterizerTraversable<Integral>::Advance();
    AdvanceTexture();
  }

  inline void AdvanceTexture() {
    UVTraversable<Integral>::AdvanceUV();
  }

  inline void Advance(const Integral value) {
    RasterizerTraversable<Integral>::Advance(value);
    AdvanceTexture(value);
  }

  inline void AdvanceTexture(const Integral value) {
    UVTraversable<Integral>::AdvanceUV(value);
  }
};

#ifndef NO_SHADING

struct RasterizerTexturedLightingData : public RasterizerTexturedData,
    public RasterizerLightingData {};

template <class Integral = unsigned> struct RasterizerTexturedLightingTraversable
    : public RasterizerTexturedTraversable<Integral>, public RasterizerLightingTraversable<Integral> {
 public:
  struct TraversableContext : public RasterizerTexturedTraversable<Integral>::TraversableContext,
      public RasterizerLightingTraversable<Integral>::TraversableContext {};

  typedef RasterizerTexturedLightingData DataType;

  typedef typename ZTraversable<Integral>::HorizontalTraversable ZHorizontal;
  typedef typename RasterizerTraversable<Integral>::HorizontalTraversable RasterizerHorizontal;
  typedef typename RasterizerTexturedTraversable<Integral>::HorizontalTraversable RasterizerTexturedHorizontal;
  typedef typename RasterizerLightingTraversable<Integral>::HorizontalTraversable RasterizerLightingHorizontal;

  class HorizontalTraversable : public RasterizerTexturedHorizontal, public RasterizerLightingHorizontal {
   public:
    typedef TraversableContext ContextType;

    inline HorizontalTraversable(ContextType *context, const ScreenLine<Integral> &a, const RasterizerTexturedLightingTraversable &da,
                                 const ScreenLine<Integral> &b, const RasterizerTexturedLightingTraversable &db, const myfloat dx) :
      ZHorizontal(a, da, b, db, dx),
      RasterizerHorizontal(context, a, da, b, db, dx),
      RasterizerTexturedHorizontal(context, a, da, b, db, dx),
      RasterizerLightingHorizontal(context, a, da, b, db, dx) {}

    inline bool Process(TraversableContext *context, const Integral x, const Integral y) {
      if (!this->z_buffer().Check(this->z())) return false;
      Uint32 pixel = this->GetPixel(context);
      sdlobj::Color tcolor = context->material->texture()->PixelToColor(pixel);
      Vector3 result = ColorToVector(tcolor);
#if defined(GOURAUD_SHADING)
      ComponentwiseInPlace(result, context->ambient + this->light());
#elif defined(FLAT_SHADING)
      ComponentwiseInPlace(result, context->light);
#else
      ComponentwiseInPlace(result, context->ambient + PhongLight(this->sources().data(),
                  *context->material, this->normal(), this->viewer(),
                  *context->light_sources));
#endif
      pixel = context->surface->ColorToPixel(VectorToColor(result));
      this->painter().SetPixel(pixel);
      return true;
    }

    inline void Advance() {
      RasterizerHorizontal::Advance();
      RasterizerTexturedHorizontal::AdvanceTexture();
      RasterizerLightingHorizontal::AdvanceLight();
    }

    inline void Advance(const Integral value) {
      RasterizerHorizontal::Advance(value);
      RasterizerTexturedHorizontal::AdvanceTexture(value);
      RasterizerLightingHorizontal::AdvanceLight(value);
    }
  };

  inline RasterizerTexturedLightingTraversable(const ScreenLine<Integral> &line, const myfloat dy,
                                       const DataType &a, const DataType &b) :
      ZTraversable<Integral>(line, dy, a.z, b.z),
      RasterizerTraversable<Integral>(line, dy, a, b),
      RasterizerTexturedTraversable<Integral>(line, dy, a, b),
      RasterizerLightingTraversable<Integral>(line, dy, a, b) {}

  inline void Advance() {
    RasterizerTraversable<Integral>::Advance();
    RasterizerTexturedTraversable<Integral>::AdvanceTexture();
    RasterizerLightingTraversable<Integral>::AdvanceLight();
  }

  inline void Advance(const Integral value) {
    RasterizerTraversable<Integral>::Advance(value);
    RasterizerTexturedTraversable<Integral>::AdvanceTexture(value);
    RasterizerLightingTraversable<Integral>::AdvanceLight(value);
  }
};

#endif

#ifndef NO_LIGHTING
typedef RasterizerTexturedLightingData RasterizerFullData;
typedef RasterizerTexturedLightingTraversable<unsigned>::TraversableContext RasterizerFullContext;
#else
typedef RasterizerTexturedTraversable::DataType RasterizerFullData;
typedef RasterizerTexturedTraversable::TraversableContext RasterizerFullContext;
#endif

template <bool kTextures, bool kLighting> void DrawTriangle(const size_t tri_i,
        const TransformedObject &cacheobject, const SceneObject &object,
        RasterizerFullContext *context) {
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

  RasterizerFullData data[points_size];

  for (size_t i = 0; i < points_size; ++i) {
    data[i].z = points_buf[i].z();
  }

  if (kLighting || kTextures) {
    for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
      const int pt = tri.points[i];
#if defined(PHONG_SHADING)
      data[i].normal = object.positioned_vertex_normals()[pt];
      data[i].viewer = cacheobject.lighting_data[pt].viewer;
      data[i].sources_data = cacheobject.lighting_data[pt].sources_data;
#elif defined(GOURAUD_SHADING)
      data[i].light = PhongLight(cacheobject.lighting_data[pt].sources_data.data(), *context->material,
                                 object.positioned_vertex_normals()[pt], cacheobject.lighting_data[pt].viewer,
                                 *context->light_sources);
#endif
      if (kTextures) {
        data[i].uv = (*object.model()->uv_coords())[tri.points[i]];
        data[i].uv.x() *= context->texture->surface()->width() - 1;
        data[i].uv.y() *= context->texture->surface()->height() - 1;
      }
    }

    if (points_size > 3) {
      bool points_needed[points_size];
      memset((bool *)&points_needed, true, IndexedTriangle::kPointsSize * sizeof(bool));
      memset((bool *)&points_needed + IndexedTriangle::kPointsSize,
             false, (points_size - IndexedTriangle::kPointsSize) * sizeof(bool));

      if (kLighting) {
#if defined(PHONG_SHADING)
        TriangleTraverser<LightDataTraversable<myfloat>> traverser(points_buf[0], data[0],
                                                        points_buf[1], data[1],
                                                        points_buf[2], data[2]);
#elif defined(GOURAUD_SHADING)
        TriangleTraverser<LightTraversable<myfloat>> traverser(points_buf[0], data[0].light,
                                                        points_buf[1], data[1].light,
                                                        points_buf[2], data[2].light);
    #endif
        for (size_t i = 0; i < triangles_size; ++i) {
          IndexedTriangle &pol = triangles_buf[i];
          if (pol.points[0] == -1) continue;
          for (size_t j = 0; j < IndexedTriangle::kPointsSize; ++j) {
            size_t pt = pol.points[j];
            if (!points_needed[pt]) {
    #if defined(PHONG_SHADING)
              Assert(traverser.Point<true>(&data[pt], points_buf[pt].x(), points_buf[pt].y()));
    #elif defined(GOURAUD_SHADING)
              Assert(traverser.Point<true>(&data[pt].light, points_buf[pt].x(), points_buf[pt].y()));
    #endif
              points_needed[pt] = true;
            }
          }
        }
      }
      if (kTextures) {
        TriangleTraverser<UVTraversable<myfloat>> traverser(points_buf[0], data[0],
                                                         points_buf[1], data[1],
                                                         points_buf[2], data[2]);
        if (kLighting) {
          for (size_t i = IndexedTriangle::kPointsSize; i < points_size; ++i) {
            if (points_needed[i]) {
              Assert(traverser.Point<true>(&data[i].uv, points_buf[i].x(), points_buf[i].y()));
            }
          }
        } else {
          for (size_t i = 0; i < triangles_size; ++i) {
            IndexedTriangle &pol = triangles_buf[i];
            if (pol.points[0] == -1) continue;
            for (size_t j = 0; j < IndexedTriangle::kPointsSize; ++j) {
              size_t pt = pol.points[j];
              if (!points_needed[pt]) {
                Assert(traverser.Point<true>(&data[pt].uv, points_buf[pt].x(), points_buf[pt].y()));
                points_needed[pt] = true;
              }
            }
          }
        }
      }
    }
  }

  for (size_t i = 0; i < triangles_size; ++i) {
    if (triangles_buf[i].points[0] == -1) continue;
    const int *const &pts = triangles_buf[i].points;
    if (kTextures && kLighting)
      TriangleTraverser<RasterizerTexturedLightingTraversable<>>::OneTraverse(context,
          points_buf[pts[0]], data[pts[0]],
          points_buf[pts[1]], data[pts[1]],
          points_buf[pts[2]], data[pts[2]]);
    else if (kTextures)
      TriangleTraverser<RasterizerTexturedTraversable<>>::OneTraverse(context,
        points_buf[pts[0]], data[pts[0]],
        points_buf[pts[1]], data[pts[1]],
        points_buf[pts[2]], data[pts[2]]);
    else if (kLighting)
      TriangleTraverser<RasterizerLightingTraversable<>>::OneTraverse(context,
          points_buf[pts[0]], data[pts[0]],
          points_buf[pts[1]], data[pts[1]],
          points_buf[pts[2]], data[pts[2]]);
    else
      TriangleTraverser<RasterizerTraversable<>>::OneTraverse(context,
          points_buf[pts[0]], data[pts[0]],
          points_buf[pts[1]], data[pts[1]],
          points_buf[pts[2]], data[pts[2]]);
  }
}

#if defined(PHONG_SHADING) || defined(GOURAUD_SHADING)

inline void FillLightingData(LightingData &lighting, const Vector3 &src, const Vector3 &camera,
                      const vector<FullLightSource> &light_sources,
                      const Vector3 &normal) {
  lighting.viewer = (camera - src).normalized();
  for (size_t i = 0; i < light_sources.size(); ++i) {
    lighting.sources_data[i].direction = (light_sources[i].position - src).normalized();
    lighting.sources_data[i].reflection = Reflection(lighting.sources_data[i].direction,
                                                     normal);
  }
}

#endif

static const myfloat system_transform_r[] = {0, 0, 1, 0,
                                             -1, 0, 0, 0,
                                             0, -1, 0, 0,
                                             0, 0, 0, 0};

static const AffineTransform system_transform = AffineTransform(Matrix4(system_transform_r));

void Rasterizer::PreRenderStep() {
#if defined(PERSPECTIVE_NORMAL_CLIPPING)
  AffineTransform rotate_transform = system_transform * context()->camera.GetRotateMatrixTo();
  AffineTransform transform = rotate_transform * context()->camera.GetTranslateMatrixTo();
#else
  AffineTransform transform = system_transform * context()->camera.GetMatrixTo();
#endif

#if defined(DUMB_NORMAL_CLIPPING)
  Vector3 normal = context()->camera.GetRotateMatrixFrom() * Position::kCameraDirection;
#endif

  TransformedObjectMap new_cache;

#if defined(PHONG_SHADING) || defined(GOURAUD_SHADING)
  Vector3 camera = context()->camera.GetVector3();
#endif

#ifndef NO_SHADING
  std::vector<FullLightSource> &light_sources = *light_sources_;
  light_sources.clear();

  for (auto &object : context()->scene.objects()) {
    if (object->light_source()) {
      FullLightSource source;
      source.position = object->position().GetVector3();
      source.diffuse = object->light_source()->diffuse;
      source.specular = object->light_source()->specular;
      light_sources.push_back(std::move(source));
    }
  }
#endif

  for (auto &object : context()->scene.objects()) {
    SceneObject *object_ptr = object.get();
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
    size_t srcpoints_s = srcpoints.size();

    hit->second->material_overlay.reset();

    Vector3Vector &points = hit->second->points;
    points.resize(srcpoints_s);

#if defined(PHONG_SHADING) || defined(GOURAUD_SHADING)
    const Vector3Vector &srcnormals = object->positioned_vertex_normals();

    std::vector<LightingData> &lighting_data = hit->second->lighting_data;
    lighting_data.resize(points.size());
    if (lighting_data[0].sources_data.size() < light_sources.size()) {
      for (auto &i : lighting_data) {
        i.sources_data = SizedRuntimeArray<LightingSourceData>(light_sources.size());
      }
    }
#endif

#ifndef NO_NORMAL_FACE_CLIPPING
    const auto &model = object->model();

    std::vector<bool> &point_flags = hit->second->point_flags;
    point_flags.assign(srcpoints_s, false);
    std::vector<size_t> &triangle_indexes = hit->second->triangle_indexes;
    triangle_indexes.clear();
    triangle_indexes.reserve(model->polygons().size() / 2);

    const Vector3Vector &pos_normals = object->positioned_polygon_normals();
#endif

#ifdef PERSPECTIVE_NORMAL_CLIPPING
    size_t pos_normals_s = pos_normals.size();
    Vector3Vector &polygon_normals = hit->second->polygon_normals;
    polygon_normals.resize(pos_normals_s);
    for (size_t i = 0; i < pos_normals_s; ++i) {
      polygon_normals[i] = rotate_transform * pos_normals[i];
    }
#endif

#if defined(NO_NORMAL_FACE_CLIPPING) || defined(PERSPECTIVE_NORMAL_CLIPPING)
    for (size_t i = 0; i < srcpoints_s; ++i) {
      points[i] = context()->camera.PerspectiveTransform(transform * srcpoints[i]);
#if defined(PHONG_SHADING) || defined(GOURAUD_SHADING)
      FillLightingData(lighting_data[i], points[i], camera, light_sources,
                       srcnormals[i]);
#endif
    }
#endif

#ifndef NO_NORMAL_FACE_CLIPPING

    auto p_i = model->polygons().begin();
    size_t i = 0;
#if defined(DUMB_NORMAL_CLIPPING)
    auto n_i = pos_normals.begin();
#elif defined(PERSPECTIVE_NORMAL_CLIPPING)
    auto n_i = polygon_normals.begin();
#endif
    for (auto pe = model->polygons().end(); p_i != pe; ++p_i, ++i) {
#if defined(DUMB_NORMAL_CLIPPING)
      if (normal.dot(*n_i++) <= 0) {
#elif defined(PERSPECTIVE_NORMAL_CLIPPING)
      const int * const &indexes = p_i->points;
      Vector3 nnormal = Vector3(points[indexes[1]] - points[indexes[0]])
          .cross(Vector3(points[indexes[2]] - points[indexes[0]]));
      bool good = (nnormal.z() <= 0) ^ (nnormal.dot(*n_i++) <= 0);
      if (good) {
#endif
        const int * const &indexes = p_i->points;
        triangle_indexes.push_back(i);
        for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
          const int &index = indexes[i];
          if (!point_flags[index]) {
#ifdef DUMB_NORMAL_CLIPPING
            points[index] = context()->camera.PerspectiveTransform(transform * srcpoints[index]);
#endif
#if defined(PHONG_SHADING) || defined(GOURAUD_SHADING)
            FillLightingData(lighting_data[index], srcpoints[index], camera, light_sources,
                             srcnormals[index]);
#endif
            point_flags[index] = true;
          }
        }
#ifndef NO_NORMAL_FACE_CLIPPING
      }
#endif
    }

#endif
  }

  *cache_ = std::move(new_cache);
}

#ifndef NO_DRAW_SURFACE
void DrawSurface(RasterizerFullContext &context, const Camera &camera,
                 const Uint32 pixel) {
  AffineTransform transform = system_transform * RotateTransform(-camera.pitch, Vector3::UnitY());

  Vector3 far(camera.view_limit(), 0, -camera.z);
  far = transform * far;
  Vector3 near(-camera.view_limit(), 0, -camera.z);
  near = transform * near;
  if (far.y() > near.y()) std::swap(far, near);
  if (far.y() < context.surface->height() && near.y() >= 0) {
    if (!Clip<ZClipper<Less<myfloat>>>(far, near, 0)) return;
    camera.PerspectiveTransformInPlace(far);
    near.y() += camera.height() / 2.0;
    if (!Clip<YClipper<Less<myfloat>>>(far, near, 0)) return;
    if (!Clip<YClipper<Greater<myfloat>>>(far, near, context.surface->height() - 1)) return;

    if (far.y() > near.y()) swap(far, near);
    int fy = near.y();
    int dy = fy - far.y();
    context.surface->FillRect(0, far.y(), context.surface->width(), dy, pixel);
    myfloat z = far.z();
    myfloat dz = 0;
    if (far.y() != fy) dz = (near.z() - z) / dy;
    ZBuffer::Iterator zi = context.z_buffer->Position(0, far.y());
    unsigned width = context.surface->width();
    for (int cy = far.y(); cy < fy; ++cy) {
      for (unsigned i = 0; i < width; ++i, ++zi) {
        *zi = z;
      }
      z += dz;
    }
    for (unsigned i = 0; i < width; ++i, ++zi) {
      *zi = z;
    }
  }
}
#endif

template <bool kTexture, bool kLighting>
  void PrepareAndDrawTemplated(RasterizerFullContext &tcontext,
                    const Material &material, SurfacePainterWrapper *&texture_painter,
                    Surface *&texture, const size_t t, const Surface &surface,
                    const TransformedObject &transformed, const SceneObject &object,
                    const Context &ccontext) {
  tcontext.material = &material;
  if (kLighting)
    tcontext.ambient = ccontext.scene.ambient_light() * material.ambient_color();
#ifdef FLAT_SHADING
  if (kLighting) {
    // getting camera vector each triangle
    Vector3 viewer = (ccontext.camera.GetVector3() - object.positioned_centers()[t]).normalized();
    const std::vector<LightSource> &light_sources = ccontext.scene.light_sources();
    const Vector3 &vertex_normal = object.positioned_polygon_normals()[t];
    RuntimeArray<LightingSourceData> sources(light_sources.size());
    for (size_t i = 0; i < ccontext.scene.light_sources().size(); ++i) {
      sources[i].direction = (light_sources[i].position - object.positioned_centers()[t]).normalized();
      sources[i].reflection = Reflection(sources[i].direction,
                                         vertex_normal);
    }
    tcontext.light = tcontext.ambient + PhongLight(sources.data(), material,
                          vertex_normal, viewer, light_sources);
    if (!kTexture) {
      tcontext.light *= 0xFF;
      tcontext.pixel = surface.ColorToPixel(VectorToColor(tcontext.light));
    }
  }
#endif
  if (kTexture) {
    if (texture != material.texture().get()) {
      if (texture_painter) delete texture_painter;
      texture = material.texture().get();
      texture_painter = GetSurfacePainter(texture);
      texture_painter->StartDrawing();
    }
    tcontext.texture = texture_painter;
  } else {
    if (!kLighting) {
      tcontext.pixel = surface.ColorToPixel(material.color());
    }
  }
  DrawTriangle<kTexture, kLighting>(t, transformed, object, &tcontext);
}

inline void PrepareAndDraw(RasterizerFullContext &tcontext,
                    const Material &material, SurfacePainterWrapper *&texture_painter,
                    Surface *&texture, const size_t t, const Surface &surface,
                    const TransformedObject &transformed, const SceneObject &object,
                    const Context &ccontext) {
  if (material.texture()) {
    if (material.lighting()) {
      PrepareAndDrawTemplated<true, true>(tcontext, material, texture_painter, texture,
                                          t, surface, transformed, object, ccontext);
    } else {
      PrepareAndDrawTemplated<true, false>(tcontext, material, texture_painter, texture,
                                          t, surface, transformed, object, ccontext);
    }
  } else {
    if (material.lighting()) {
      PrepareAndDrawTemplated<false, true>(tcontext, material, texture_painter, texture,
                                          t, surface, transformed, object, ccontext);
    } else {
      PrepareAndDrawTemplated<false, false>(tcontext, material, texture_painter, texture,
                                          t, surface, transformed, object, ccontext);
    }
  }
}

void Rasterizer::Paint(sdlobj::Surface &surface) {
  z_buffer_.set_size(surface.width(), surface.height());
  z_buffer_.Clear();

  SurfacePainterWrapper *surface_painter = GetSurfacePainter(&surface);

  RasterizerFullContext tcontext;
  tcontext.surface = &surface;
  tcontext.z_buffer = &z_buffer_;
  tcontext.surface_painter = surface_painter;
#if defined(GOURAUD_SHADING) || defined(PHONG_SHADING)
  tcontext.light_sources = light_sources_.get();
#endif

#ifndef NO_DRAW_SURFACE
  DrawSurface(tcontext, context()->camera, surface.ColorToPixel(surface_color_));
#endif

  SurfacePainterWrapper *texture_painter = nullptr;
  Surface *texture = nullptr;

  surface_painter->StartDrawing();
  for (auto &cacheobject : *cache_) {
    const TransformedObject &transformed = *cacheobject.second;
    std::shared_ptr<SceneObject> object = transformed.object.lock();
#ifdef NO_NORMAL_FACE_CLIPPING
    size_t tri_s = object->model()->polygons().size();
#endif
    if (transformed.material_overlay) {
      MaterialOverlayMap *map = transformed.material_overlay.get();
      auto m_end = map->end();
#ifndef NO_NORMAL_FACE_CLIPPING
      for (const auto &t : transformed.triangle_indexes) {
#else
      for (size_t t = 0; t < tri_s; ++t) {
#endif
        auto m_i = map->find(t);
        const Material *material;
        if (m_i == m_end) {
          material = object->model()->materials()[t].get();
        } else {
          material = m_i->second.get();
        }
        PrepareAndDraw(tcontext, *material, texture_painter, texture, t,
                       surface, transformed, *object, *context());
      }
    } else {
#ifndef NO_NORMAL_FACE_CLIPPING
      for (const auto &t : transformed.triangle_indexes) {
#else
      for (size_t t = 0; t < tri_s; ++t) {
#endif
        const Material *material = object->model()->materials()[t].get();
        PrepareAndDraw(tcontext, *material, texture_painter, texture, t,
                       surface, transformed, *object, *context());
      }
    }
  }

  if (texture_painter) delete texture_painter;
  delete surface_painter;
}

void Rasterizer::set_surface_color(const sdlobj::Color &surface_color) {
  surface_color_ = surface_color;
}

void Rasterizer::Clear() {
  cache_->clear();
}
