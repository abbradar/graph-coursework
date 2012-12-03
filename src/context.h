#ifndef GRAPH_CONTEXT_H_
#define GRAPH_CONTEXT_H_

#include <list>
#include <memory>
#include <map>
#include "scene.h"
#include "models.h"
#include "camera.h"

class Window;

typedef std::map<size_t, std::shared_ptr<Material>> MaterialOverlayMap;

struct TransformedObject {
  TransformedObject(const std::weak_ptr<SceneObject> &object);

  std::weak_ptr<SceneObject> object;
  Point3DVector points;
  std::vector<bool> point_flags;
  std::vector<size_t> triangle_indexes;
  std::unique_ptr<MaterialOverlayMap> material_overlay;
};

typedef std::map<SceneObject *, std::unique_ptr<TransformedObject>> TransformedObjectMap;
typedef std::pair<SceneObject *, std::unique_ptr<TransformedObject>> TransformedObjectPair;

struct TracedObject {
  std::weak_ptr<SceneObject> object;
  size_t triangle_index;
  unsigned x, y;
  myfloat z;
};

struct Context {
 public:
  Context(Window *window);

  Scene scene;
  Models models;
  Camera camera;
  std::weak_ptr<TransformedObjectMap> transformed_objects;
  std::weak_ptr<TracedObject> traced_object;

  Window *window;
};

class ContextUser {
 public:
  ContextUser(const std::shared_ptr<Context> &context);

  inline const std::shared_ptr<Context> &context() {
    return context_;
  }

 private:
  std::shared_ptr<Context> context_;
};

#include "window.h"

#endif // GRAPH_CONTEXT_H_
