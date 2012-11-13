#ifndef GRAPH_CONTEXT_H_
#define GRAPH_CONTEXT_H_

#include <list>
#include <memory>
#include <map>
#include "scene.h"
#include "models.h"
#include "position.h"

class Window;

struct TransformedObject {
  TransformedObject(const std::weak_ptr<SceneObject> &object);

  std::weak_ptr<SceneObject> object;
  Point3DVector points;
  std::vector<bool> point_flags;
  TriangleVector triangles;
  std::vector<size_t> triangle_indexes;
};

typedef std::map<SceneObject *, std::unique_ptr<TransformedObject>> TransformedObjectMap;
typedef std::pair<SceneObject *, std::unique_ptr<TransformedObject>> TransformedObjectPair;

struct Context {
 public:
  Context(Window *window);

  Scene scene;
  Models models;
  Position position;
  std::weak_ptr<const TransformedObjectMap> transformed_objects;
  Window *window;
};

class ContextUser {
 public:
  ContextUser(const std::shared_ptr<Context> &context);

  inline const std::shared_ptr<Context> context() {
    return context_;
  }

 private:
  std::shared_ptr<Context> context_;
};

#include "window.h"

#endif // GRAPH_CONTEXT_H_
