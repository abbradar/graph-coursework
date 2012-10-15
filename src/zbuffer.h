#ifndef GRAPH_ZBUFFER_H_
#define GRAPH_ZBUFFER_H_

#include <cstring>
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
    myfloat *p = z_buffer_ + y * width_ + x;
    if (*p < z) {
      return false;
    } else {
      *p = z;
      return true;
    }
  }

  inline myfloat &at(const unsigned int x, const unsigned int y) {
    return z_buffer_[y * width_ + x];
  }

 private:
  unsigned int width_;
  unsigned int height_;
  size_t size_;
  myfloat *z_buffer_;
};

#endif // GRAPH_ZBUFFER_H_
