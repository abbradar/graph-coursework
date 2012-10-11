#include <limits>
#include "zbuffer.h"

using namespace std;

ZBuffer::ZBuffer(unsigned int width, unsigned int height) : width_(width), height_(height),
  size_(width_ * height_), z_buffer_(new myfloat[size_]) {}

ZBuffer::~ZBuffer() {
  delete[] z_buffer_;
}

void ZBuffer::set_size(const unsigned int width, const unsigned int height) {
  size_t new_size = width * height;
  if (size_ != new_size) {
    delete[] z_buffer_;
    z_buffer_ = new myfloat[new_size];
  }
  width_ = width;
  height_ = height;
  size_ = new_size;
}

void ZBuffer::clear() {
  memset(z_buffer_, 0xFF, sizeof(myfloat) * size_);
}
