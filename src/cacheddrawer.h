#ifndef GRAPH_CONTROL_H_
#define GRAPH_CONTROL_H_

#include "sdlobj/surface.h"
#include "renderworker.h"

class CachedDrawer : virtual public RenderWorker {
 public:
  CachedDrawer();
  virtual ~CachedDrawer();

  inline unsigned x() {
    return x_;
  }

  inline unsigned y() {
    return y_;
  }

  virtual void set_position(const unsigned x, const unsigned y);

  inline unsigned width() {
    return width_;
  }

  inline unsigned height() {
    return height_;
  }

  virtual void set_size(const unsigned width, const unsigned height);

  virtual unsigned preferred_width();
  virtual unsigned preferred_height();

  inline bool valid() {
    return valid_;
  }

  void Invalidate();

  const sdlobj::Surface &surface() {
    return surface_;
  }

  virtual void Paint(sdlobj::Surface &surface);

 protected:
  virtual void Repaint(sdlobj::Surface &surface) = 0;

 private:
  unsigned x_, y_, width_, height_;
  bool valid_;
  sdlobj::Surface surface_;
};

#endif // GRAPH_CONTROL_H_
