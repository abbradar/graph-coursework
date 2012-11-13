#include <boost/format.hpp>
#include "positionlabel.h"

using namespace sdlobj;
using namespace std;

PositionLabel::PositionLabel(const std::shared_ptr<Context> &context) :
  ContextUser(context) {}

void PositionLabel::Paint(sdlobj::Surface &surface) {
  string position_str = (boost::format("Position: x: %1%, y: %2%, z: %3%, pitch: %4%, yaw: %5%")
                % context()->position.x % context()->position.y
                % context()->position.z % context()->position.pitch
                % context()->position.yaw).str();
  Surface rendered = font().RenderUTF8_Solid(position_str.data(), font_color());
  surface.Blit(rendered, x_, y_);
}

void PositionLabel::set_position(const unsigned x, const unsigned y) {
  x_ = x;
  y_ = y;
}
