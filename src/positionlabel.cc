#include <boost/format.hpp>
#include "positionlabel.h"

using namespace sdlobj;
using namespace std;

PositionLabel::PositionLabel(const std::shared_ptr<Context> &context) :
 ContextUser(context) {
  set_label((boost::format("Position: x: %1%, y: %2%, z: %3%, pitch: %4%, yaw: %5%")
               % context->camera.x % context->camera.y
               % context->camera.z % context->camera.pitch
               % context->camera.yaw).str());
}

void PositionLabel::Paint(sdlobj::Surface &surface) {
  set_label((boost::format("Position: x: %1%, y: %2%, z: %3%, pitch: %4%, yaw: %5%")
                % context()->camera.x % context()->camera.y
                % context()->camera.z % context()->camera.pitch
                % context()->camera.yaw).str());
  FastLabelControl::Paint(surface);
}
