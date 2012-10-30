#include <fstream>
#include "common/math.h"
#include "xparse/xfile.h"
#include "scene.h"

using namespace sdlobj;
using namespace xparse;

const Point3D test_points[] = { Point3D(0, 0, 0), Point3D(100, 0, 0), Point3D(0, 0, 100),
                                Point3D(0, 100, 0), Point3D(100, 100, 0), Point3D(0, 100, 100),
                                Point3D(100, 0, 100), Point3D(100, 100, 100) };
const IndexedTriangle test_indexes[] = { IndexedTriangle(0, 1, 2), IndexedTriangle(1, 2, 6),
                                         IndexedTriangle(0, 1, 3), IndexedTriangle(1, 3, 4),
                                         IndexedTriangle(0, 2, 3), IndexedTriangle(2, 3, 5),
                                         IndexedTriangle(7, 6, 5), IndexedTriangle(6, 5, 2),
                                         IndexedTriangle(7, 6, 4), IndexedTriangle(6, 4, 1),
                                         IndexedTriangle(7, 5, 4), IndexedTriangle(5, 4, 3) };
const Point3D test_vertex_normals[] = { Point3D(-1, -1, -1), Point3D(1, -1, -1), Point3D(-1, -1, 1),
                                        Point3D(-1, 1, -1), Point3D(1, 1, -1), Point3D(-1, 1, 1),
                                        Point3D(1, -1, 1), Point3D(1, 1, 1) };

Scene::Scene() = default;

void Scene::set_plane_color(const Color &plane_color) {
  plane_color_ = plane_color;
}

void Scene::Load() {
  // test object
  PointVector points(test_points, test_points + 8);
  TriangleVector polygons(test_indexes, test_indexes + 12);
  PointVector vertex_normals(test_vertex_normals, test_vertex_normals + 8);
  SceneObject object(points, polygons, vertex_normals, Position(0, 0, 0));
  object.color() = Color(0, 0xFF, 0);
  objects_.push_back(std::move(object));

  XFile file;
  std::ifstream in("cube.x");
  file.Parse(in);
}
