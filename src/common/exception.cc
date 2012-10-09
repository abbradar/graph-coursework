#include "stacktrace.h"
#include "exception.h"

Exception::Exception(const std::string &what) : what_(what) {
  stack_trace_ = StackTrace();
}

const char *Exception::what() const noexcept {
  return what_.data();
}

const char *Exception::stack_trace() const noexcept {
  return stack_trace_.data();
}
