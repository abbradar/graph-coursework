#include <boost/format.hpp>
#include "mousepositionlabel.h"

using namespace std;

MousePositionLabel::MousePositionLabel(const std::shared_ptr<Context> &context)
 : ContextUser(context) {
  set_label((boost::format("x: %1%, y: %2%") % context->window->cursor_x()
             % context->window->cursor_y()).str());
}

void MousePositionLabel::EventStep() {
  set_show(!context()->window->grab_input());
  if (show()) {
    set_label((boost::format("x: %1%, y: %2%") % context()->window->cursor_x()
               % context()->window->cursor_y()).str());
  }
}
