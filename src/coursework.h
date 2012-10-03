#ifndef GRAPH_COURSEWORK_H_
#define GRAPH_COURSEWORK_H_

#include "common/application.h"
#include "sdlobj/color.h"

class CourseWork : public Runnable
{
 public:
  static const char *const kProgramName;

  CourseWork();
  CourseWork(const CourseWork &other) = delete;
  CourseWork(const CourseWork &&other) = delete;

 private:
  ~CourseWork();

  int Run(int argc, const char **argv);
  void Terminate(int error_code) noexcept;
};

#endif // GRAPH_COURSEWORK_H_
