#ifndef POINTERDRAWER_H
#define POINTERDRAWER_H

#include "sdlobj/surface.h"
#include "renderworker.h"
#include "context.h"

class PointerDrawer : public RenderWorker, public ContextUser {
 public:
  PointerDrawer(const std::shared_ptr<Context> &context);

  inline const sdlobj::Surface &pointer() {
    return pointer_;
  }

  void set_pointer(const sdlobj::Surface &pointer);

  void Paint(sdlobj::Surface &surface);

 private:
  sdlobj::Surface pointer_;
};

#endif // POINTERDRAWER_H
