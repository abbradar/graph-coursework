#ifndef GRAPH_SCENEOBJECT_H_
#define GRAPH_SCENEOBJECT_H_

#include <vector>
#include <memory>
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
  SceneObject(const PointVector &points, const TriangleVector &polygons, const Position &position);

  /** Points vector */
  inline const PointVector &points() const {
    return points_;
  }

  inline const TriangleVector &polygons() const {
    return polygons_;
  }

  void set_model(const PointVector &points, const TriangleVector &polygons);

  inline const PointVector &positioned_points() const {
    return positioned_points_;
  }

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
  TriangleVector polygons_;
  Position position_;
  sdlobj::Color color_;

  void UpdatePositioned();
  void PopulateTransformed();
};

#endif // GRAPH_SCENEOBJECT_H_
