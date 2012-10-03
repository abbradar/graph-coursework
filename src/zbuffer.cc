#include <limits>
#include "zbuffer.h"

using namespace std;

ZBuffer::ZBuffer(int width, int height) : width_(width), height_(height),
  size_(width_ * height_), z_buffer_(new myfloat[size_]) {}

ZBuffer::~ZBuffer() {
  delete[] z_buffer_;
}

void ZBuffer::set_size(const int width, const int height) {
  int new_size = width * height;
  if (size_ != new_size) {
    delete[] z_buffer_;
    z_buffer_ = new myfloat[new_size];
  }
  width_ = width;
  height_ = height;
  size_ = new_size;
}

void ZBuffer::clear() {
  const myfloat val = numeric_limits<myfloat>::has_infinity ?
        numeric_limits<myfloat>::infinity() :
        numeric_limits<myfloat>::max();
  for (int i = 0; i < size_; ++i) {
    z_buffer_[i] = val;
  }
}
