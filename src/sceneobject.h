#ifndef GRAPH_SCENEOBJECT_H_
#define GRAPH_SCENEOBJECT_H_

#include <vector>
#include "sdlobj/color.h"
#include "matrix4.h"
#include "point3d.h"
#include "position.h"
#include "indexedpolygon.h"

/** Scene object */
class SceneObject {
 public:
  typedef std::vector<Point3D> PointVector;
  typedef std::vector<IndexedPolygon> PolygonVector;

  SceneObject();
  SceneObject(const PointVector &points, const PolygonVector &polygons, const Position &position);

  /** Points vector */
  inline const PointVector &points() const {
    return points_;
  }

  inline const PolygonVector &polygons() const {
    return polygons_;
  }

  void set_model(const PointVector &points, const PolygonVector &polygons);

  inline const PointVector &positioned_points() const {
    return positioned_points_;
  }

  inline const PointVector &transformed_points() const {
    return transformed_points_;
  }

  void ApplyTransform(const Matrix4 &transform);
  void ClearTransformed();
  bool ClearedTransformed();

  inline const Position &position() const {
    return position_;
  }

  void set_position(const Position &position);

  inline sdlobj::Color &color() {
    return color_;
  }

 private:
  PointVector points_;
  PointVector positioned_points_;
  PointVector transformed_points_;
  PolygonVector polygons_;
  Position position_;
  sdlobj::Color color_;

  void UpdatePositioned();
  void PopulateTransformed();
};

#endif // GRAPH_SCENEOBJECT_H_
