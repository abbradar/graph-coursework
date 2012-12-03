#ifndef GRAPH_ZBUFFER_H_
#define GRAPH_ZBUFFER_H_

#include <cstring>
#include "common/debug.h"
#include "myfloat.h"

class ZBuffer {
 public:
  typedef myfloat ZBufferType;
  ZBuffer(const unsigned int width, const unsigned int height);
  ~ZBuffer();

  inline unsigned int width() {
    return width_;
  }

  inline unsigned int height() {
    return height_;
  }

  void set_size(const unsigned int width, const unsigned int height);

  void Clear();

  inline bool Check(const unsigned int x, const unsigned int y, const myfloat z) {
#if DEBUG_LEVEL >= 4
    Assert(x < width_ && y < height_);
#endif
    myfloat *p = z_buffer_ + y * width_ + x;
    if (*p < z) {
      return false;
    } else {
      *p = z;
      return true;
    }
  }

  inline myfloat &At(const unsigned int x, const unsigned int y) {
#if DEBUG_LEVEL >= 4
    Assert(x < width_ && y < height_);
#endif
    return z_buffer_[y * width_ + x];
  }

  void FillLine(const unsigned int y, const myfloat z) {
    for (myfloat *i = z_buffer_ + y * width_; i < z_buffer_ + (y + 1) * width_; ++i) {
      *i = z;
    }
  }

 private:
  unsigned int width_;
  unsigned int height_;
  size_t size_;
  myfloat *z_buffer_;
};

#endif // GRAPH_ZBUFFER_H_
