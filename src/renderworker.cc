#include "renderworker.h"

RenderWorker::RenderWorker() : show_(true) {}

RenderWorker::~RenderWorker() = default;

bool RenderWorker::show() {
  return show_;
}

void RenderWorker::set_show(const bool show) {
  show_ = show;
}

RenderWorker *RenderWorker::render_worker() {
  return this;
}
