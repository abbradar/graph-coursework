#include <boost/format.hpp>
#include "common/exception.h"
#include "fpslabel.h"

using namespace sdlobj;
using namespace std;

FPSLabel::FPSLabel(const std::shared_ptr<Context> &context) :
 ContextUser(context), fps_step_(1), curr_step_(0) {
  set_label((boost::format("%.1f fps") % context->window->measured_fps()).str());
}

void FPSLabel::EventStep() {
  if (curr_step_ == 0) {
    Invalidate();
    curr_step_ = fps_step_;
  }
  --curr_step_;
}

void FPSLabel::Repaint(Surface &surface) {
  set_label((boost::format("%.1f fps") % context()->window->measured_fps()).str());
  surface = font().RenderUTF8_Solid(label().data(), font_color());
}

void FPSLabel::set_fps_step(const unsigned fps_step) {
  if (fps_step == 0) {
    throw Exception("FPS step should be positive");
  }
  fps_step_ = fps_step;
}
