#include "frametimersettings.h"

void operator >>(const YAML::Node &node, sdlobj::FrameTimer &frame_timer) {
  float fps;
  node["fps"] >> fps;
  frame_timer.set_fps(fps);
  bool measure_fps;
  node["measure-fps"] >> measure_fps;
  frame_timer.set_measure_fps(measure_fps);
  int measure_ticks;
  node["measure-ticks"] >> measure_ticks;
  frame_timer.set_measure_ticks(measure_ticks);
}
