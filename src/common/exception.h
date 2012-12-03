#ifndef COMMON_EXCEPTION_H_
#define COMMON_EXCEPTION_H_

#include <exception>
#include <string>

class Exception : public std::exception {
 public:
  explicit Exception(const std::string &what);
  const char *what() const noexcept;
  const char *stack_trace() const noexcept;

 private:
  std::string what_;
  std::string stack_trace_;
};

#endif // COMMON_EXCEPTION_H_
