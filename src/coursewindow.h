#ifndef COURSEWINDOW_H
#define COURSEWINDOW_H

#include "drawingwindow.h"

class CourseWindow : public DrawingWindow
{
public:
  CourseWindow() = default;

  void ProcessQuit(const SDL_QuitEvent &);
};

#endif // COURSEWINDOW_H
