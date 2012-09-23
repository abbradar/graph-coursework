#ifndef GRAPH_COURSEWORK_H_
#define GRAPH_COURSEWORK_H_

#include "common/application.h"
#include "fontmanager.h"

class CourseWork : public Runnable
{
 public:
  static constexpr const char *kProgramName = "Course Work";
  static constexpr const char *kDefaultFontName = "DroidSans.ttf";
  static constexpr int kDefaultFontIndex = 0;
  static constexpr int kDefaultFontSize = 12;
  static const SDL_Color kDefaultFontColor;

  CourseWork() = default;
  CourseWork(const CourseWork &other) = delete;
  CourseWork(const CourseWork &&other) = delete;

 private:
  ~CourseWork() = default;

  const int tick_ = 1000 / 60;
  FontManager font_manager_;
  sdlobj::Font default_font_;

  int Run(int argc, const char **argv);
  void Terminate(int error_code) noexcept;
};

#endif // GRAPH_COURSEWORK_H_
