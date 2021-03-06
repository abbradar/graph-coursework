#ifndef COMMON_RUNTIMEARRAY_H_
#define COMMON_RUNTIMEARRAY_H_

#include <cstdlib>
#include <utility>
#include "common/exception.h"

template <class T> class RuntimeArray {
 public:
  inline RuntimeArray() : data_(nullptr) {}

  inline RuntimeArray(const size_t size) {
    data_ = new T[size];
    if (!data_) throw Exception("Memory allocation error");
  }

  inline RuntimeArray(T *&&data) {
    data_ = data;
  }

  inline RuntimeArray(const T *data, const size_t size) {
    Copy(data, size);
  }

  inline ~RuntimeArray() {
    Destroy();
  }

  void Assign(T *&&data) {
    Destroy();
    data_ = data;
  }

  void Assign(const T *data, const size_t size) {
    Destroy();
    Copy(data, size);
  }

  void Assign(RuntimeArray &&other) {
    Assign(other.data());
    other.data_ = nullptr;
  }

  void Assign(const RuntimeArray &other, const size_t size) {
    Assign(other.data(), size);
  }

  RuntimeArray &operator =(const RuntimeArray<T> &other) = delete;

  inline RuntimeArray &operator =(RuntimeArray<T> &&other) {
    Assign(std::forward<RuntimeArray<T>>(other));
    return *this;
  }

  RuntimeArray(const RuntimeArray<T> &other) = delete;

  RuntimeArray(RuntimeArray<T> &&other) {
    data_ = other.data_;
    other.data_ = nullptr;
  }

  inline T *data() {
    return data_;
  }

  inline const T *data() const {
    return data_;
  }

  inline T &operator [](const size_t i) {
    return data_[i];
  }

  inline const T &operator [](const size_t i) const {
    return data_[i];
  }

 private:
  inline void Copy(const T *data, const size_t size) {
    data_ = new T[size];
    if (!data_) throw Exception("Memory allocation error");
    for (size_t i = 0; i < size; ++i) {
      data_[i] = data[i];
    }
  }

  inline void Destroy() {
    delete[] data_;
  }

  T *data_;
};

template <class T> class SizedRuntimeArray : private RuntimeArray<T> {
 public:
  inline SizedRuntimeArray() : RuntimeArray<T>(), size_(0) {}

  inline SizedRuntimeArray(const size_t size) : RuntimeArray<T>(size), size_(size) {}

  inline SizedRuntimeArray(T *&&array, const size_t size) :
    RuntimeArray<T>(std::forward(array)), size_(size) {}

  inline SizedRuntimeArray(const T *array, const size_t size) :
    RuntimeArray<T>(array, size), size_(size) {}

  SizedRuntimeArray(const SizedRuntimeArray<T> &other) :
    RuntimeArray<T>(other.data(), other.size()), size_(other.size()) {}

  SizedRuntimeArray(SizedRuntimeArray<T> &&other) = default;

  inline size_t size() const {
    return size_;
  }

  inline T *data() {
    return RuntimeArray<T>::data();
  }

  inline const T *data() const {
    return RuntimeArray<T>::data();
  }

  inline T &operator [](const size_t i) {
    return RuntimeArray<T>::operator [](i);
  }

  inline const T &operator [](const size_t i) const {
    return RuntimeArray<T>::operator [](i);
  }

  void Assign(T *&&data, const size_t size) {
    RuntimeArray<T>::Assign(std::forward<T *>(data));
    size_ = size;
  }

  void Assign(const T *data, const size_t size) {
    RuntimeArray<T>::Assign(data, size);
    size_ = size;
  }

  void Assign(SizedRuntimeArray<T> &&other) {
    RuntimeArray<T>::Assign(std::forward<SizedRuntimeArray<T>>(other));
    size_ = other.size_;
  }

  void Assign(const SizedRuntimeArray<T> &other) {
    RuntimeArray<T>::Assign(other, other.size());
    size_ = other.size_;
  }

  void Assign(RuntimeArray<T> &&other, const size_t size) {
    RuntimeArray<T>::Assign(std::forward<RuntimeArray<T>>(other));
    size_ = size;
  }

  void Assign(const RuntimeArray<T> &other, const size_t size) {
    RuntimeArray<T>::Assign(other, size);
    size_ = size;
  }

  SizedRuntimeArray &operator =(const SizedRuntimeArray<T> &other) {
    Assign(other);
    return *this;
  }

  inline SizedRuntimeArray &operator =(SizedRuntimeArray<T> &&other) {
    Assign(std::forward<SizedRuntimeArray<T>>(other));
    return *this;
  }

 private:
  size_t size_;
};

#endif // COMMON_RUNTIMEARRAY_H_
