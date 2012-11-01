#ifndef GRAPH_FRAMETIMERSETTINGS_H_
#define GRAPH_FRAMETIMERSETTINGS_H_

#include <yaml-cpp/node.h>
#include "sdlobj/frametimer.h"

void operator >>(const YAML::Node &node, sdlobj::FrameTimer &frame_timer);

#endif // GRAPH_FRAMETIMERSETTINGS_H_
