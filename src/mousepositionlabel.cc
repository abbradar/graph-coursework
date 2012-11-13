#include <boost/format.hpp>
#include "mousepositionlabel.h"

using namespace std;

MousePositionLabel::MousePositionLabel(const std::shared_ptr<Context> &context)
 : ContextUser(context), x_(0), y_(0) {
  label() = string(16, ' ');
}

void MousePositionLabel::EventStep() {
  set_show(!context()->window->show_cursor());
  if (show()) {
    label() = (boost::format("x: %1%, y: %2%") % context()->window->cursor_x() % context()->window->cursor_y()).str();
  }
}
