#include "conveyorworker.h"

ConveyorWorker::ConveyorWorker() = default;

ConveyorWorker::~ConveyorWorker() = default;

EventWorker *ConveyorWorker::event_worker() {
  return nullptr;
}

PreRenderWorker *ConveyorWorker::pre_render_worker() {
  return nullptr;
}

RenderWorker *ConveyorWorker::render_worker() {
  return nullptr;
}

PostRenderWorker *ConveyorWorker::post_render_worker() {
  return nullptr;
}

PreRenderWorker *PreRenderWorker::pre_render_worker() {
  return this;
}

PostRenderWorker *PostRenderWorker::post_render_worker() {
  return this;
}
