#include "pointerdrawer.h"

PointerDrawer::PointerDrawer(const std::shared_ptr<Context> &context) :
  ContextUser(context) {}

void PointerDrawer::set_pointer(const sdlobj::Surface &pointer) {
  pointer_ = pointer;
}

void PointerDrawer::Paint(sdlobj::Surface &surface) {
  if (context()->window->show_cursor()) {
    surface.Blit(pointer_, (context()->window->cursor_x() - pointer_.width()) / 2,
                 (context()->window->cursor_y() - pointer_.height()) / 2);
  }
}
