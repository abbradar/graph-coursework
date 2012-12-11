#include "common/debug.h"
#include "config.h"
#include "contexttracer.h"

using namespace std;

ContextTracer::ContextTracer(const std::shared_ptr<Context> &context) :
 ContextUser(context), traced_object_(new TracedObject()),
 material_(new Material()) {
  context->traced_object = traced_object_;
}

void ContextTracer::PreRenderStep() {
  tracer_.set_point(context()->window->cursor_x(), context()->window->cursor_y());
  auto objects = context()->transformed_objects.lock();
  std::shared_ptr<SceneObject> traced;
  size_t index = 0;
  for (auto &i : *objects) {
    auto object = i.second->object.lock();
    const TriangleVector &polygons = object->model()->polygons();
#ifndef NO_NORMAL_FACE_CLIPPING
    for (auto &j : i.second->triangle_indexes) {
#else
    size_t psize = polygons.size();
    for (size_t j = 0; j < psize; ++j) {
#endif
      if(tracer_.TraceNext(polygons[j], i.second->points)) {
        traced = object;
        index = j;
      }
    }
  }
  traced_object_->object = traced;
  if (traced) {
    traced_object_->triangle_index = index;
    traced_object_->x = tracer_.x();
    traced_object_->y = tracer_.y();
    traced_object_->z = tracer_.z();
    auto cache = objects->find(traced.get());
    MaterialOverlayMap *overlay = new MaterialOverlayMap();
    overlay->insert(pair<size_t, std::shared_ptr<Material>>(index, material_));
    (*cache).second->material_overlay.reset(overlay);
  }
}
