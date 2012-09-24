#ifndef GRAPH_COURSEWORK_H_
#define GRAPH_COURSEWORK_H_

#include "common/application.h"
#include "fontmanager.h"

class CourseWork : public Runnable
{
 public:
  static const char * const kProgramName;
  static const char * const kDefaultFontName;
  static const int kDefaultFontIndex;
  static const int kDefaultFontSize;
  static const SDL_Color kDefaultFontColor;

  CourseWork();
  CourseWork(const CourseWork &other) = delete;
  CourseWork(const CourseWork &&other) = delete;

 private:
  ~CourseWork() = default;

  int tick_;
  FontManager font_manager_;
  sdlobj::Font default_font_;

  int Run(int argc, const char **argv);
  void Terminate(int error_code) noexcept;
};

#endif // GRAPH_COURSEWORK_H_
