#ifndef GRAPH_SCENEOBJECT_H_
#define GRAPH_SCENEOBJECT_H_

#include <vector>
#include <string>
#include <memory>
#include "xparse/xdata.h"
#include "sdlobj/color.h"
#include "matrix4.h"
#include "point3d.h"
#include "position.h"
#include "indexedtriangle.h"

typedef std::vector<Point3D> PointVector;
typedef std::vector<IndexedTriangle> TriangleVector;

/** Scene object */
class SceneObject {
 public:
  SceneObject();
  SceneObject(const PointVector &points, const TriangleVector &polygons,
              const PointVector &vertex_normals, const Position &position);

  static SceneObject LoadFromFrame(xparse::XData *frame);

  inline std::string &name() {
    return name_;
  }

  /** Points vector */
  inline const PointVector &points() const {
    return points_;
  }

  inline const TriangleVector &polygons() const {
    return polygons_;
  }

  void set_model(const PointVector &points, const TriangleVector &polygons,
                 const PointVector &vertex_normals);

  inline const PointVector &positioned_points() const {
    return positioned_points_;
  }

  inline const PointVector &positioned_polygon_normals() const {
    return positioned_polygon_normals_;
  }

  inline const PointVector &vertex_normals() const {
    return vertex_normals_;
  }

  inline const PointVector &polygon_normals() const {
    return polygon_normals_;
  }

  inline const Position &position() const {
    return position_;
  }

  void set_position(const Position &position);

  inline sdlobj::Color &color() {
    return color_;
  }

 private:
  std::string name_;
  PointVector points_;
  PointVector positioned_points_;
  TriangleVector polygons_;
  PointVector vertex_normals_;
  PointVector polygon_normals_;
  PointVector positioned_polygon_normals_;
  Position position_;
  sdlobj::Color color_;

  void UpdatePositioned();
  void PopulateTransformed();
  void ComputePolygonNormals();
};

#endif // GRAPH_SCENEOBJECT_H_
