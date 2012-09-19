#ifndef GRAPH_COURSEWORK_H_
#define GRAPH_COURSEWORK_H_

#include "application.h"

class CourseWork : public Runnable
{
 public:
  CourseWork() = default;
  CourseWork(const CourseWork &other) = delete;
  CourseWork(const CourseWork &&other) = delete;

 private:
  ~CourseWork() = default;

  virtual int Run(int argc, const char **argv);
  virtual void Terminate(int error_code) noexcept;
};

#endif // GRAPH_COURSEWORK_H_