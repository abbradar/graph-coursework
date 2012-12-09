#include "zbuffer.h"

#ifdef MULTITHREADED_Z_BUFFER
#include <condition_variable>
#endif

using namespace std;

ZBuffer::ZBuffer(unsigned int width, unsigned int height) : width_(width),
    height_(height), size_(width_ * height_), z_buffer_(new myfloat[size_])
#ifdef MULTITHREADED_Z_BUFFER
    , z_buffer_other_(new myfloat[size_]), thread_(bind(&ZBuffer::ClearOtherThread, this))
#endif
{}

ZBuffer::~ZBuffer() {
#ifdef MULTITHREADED_Z_BUFFER
  v_from_.lock();
#endif
  delete[] z_buffer_;
#ifdef MULTITHREADED_Z_BUFFER
  delete[] z_buffer_other_;
  z_buffer_other_ = nullptr;
  v_from_.unlock();
  v_to_.notify_one();
  thread_.join();
#endif
}

void ZBuffer::set_size(const unsigned int width, const unsigned int height) {
  size_t new_size = width * height;
  if (size_ != new_size) {
#ifdef MULTITHREADED_Z_BUFFER
    v_from_.lock();
#endif
    size_ = new_size;
    delete[] z_buffer_;
    z_buffer_ = new myfloat[size_];
#ifdef MULTITHREADED_Z_BUFFER
    delete[] z_buffer_other_;
    z_buffer_other_ = new myfloat[size_];
    v_from_.unlock();
    v_to_.notify_one();
#endif
  }
  width_ = width;
  height_ = height;
}

void ZBuffer::Clear() {
#ifdef MULTITHREADED_Z_BUFFER
  v_from_.lock();
  std::swap(z_buffer_, z_buffer_other_);
  v_from_.unlock();
  v_to_.notify_one();
#else
  memset(z_buffer_, 0x7F, size_ * sizeof(myfloat));
#endif
}

ZBuffer::Iterator ZBuffer::Position(const unsigned int x, const unsigned int y) {
  return z_buffer_ + y * width_ + x;
}

#ifdef MULTITHREADED_Z_BUFFER
void ZBuffer::ClearOtherThread() {
  //static const myfloat max = std::numeric_limits<myfloat>::max();
  std::mutex m_to;
  std::unique_lock<std::mutex> lock(m_to);
  while (z_buffer_other_) {
    v_from_.lock();
    //std::fill_n(z_buffer_other_, size_, max);
    memset(z_buffer_other_, 0x7F, size_ * sizeof(myfloat));
    v_from_.unlock();
    v_to_.wait(lock);
  }
}
#endif

ZBuffer::Iterator::Iterator()  = default;

ZBuffer::Iterator::Iterator(myfloat *pos) : pos_(pos) {}
