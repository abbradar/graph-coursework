#ifndef GRAPH_ZBUFFER_H_
#define GRAPH_ZBUFFER_H_

#include "myfloat.h"

class ZBuffer {
 public:
  ZBuffer(int width, int height);
  ~ZBuffer();

  inline int width() {
    return width_;
  }

  inline int height() {
    return height_;
  }

  void set_size(int width, int height);

  void clear();

  inline bool visible(int x, int y, myfloat z) {
    myfloat *p = z_buffer_ + y * width_ + x;
    if (*p <= z) {
      return false;
    } else {
      *p = z;
      return true;
    }
  }

  inline myfloat &at(int x, int y) {
    return z_buffer_[y * width_ + x];
  }

 private:
  int width_;
  int height_;
  int size_;
  myfloat *z_buffer_;
};

#endif // GRAPH_ZBUFFER_H_
