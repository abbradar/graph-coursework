#ifndef GRAPH_LABELCONTROL_H_
#define GRAPH_LABELCONTROL_H_

#include <string>
#include "textcontrol.h"

class LabelControl : virtual public TextControl {
 public:
  LabelControl();

  inline const std::string &label() const {
    return label_;
  }

  unsigned preferred_height();
  unsigned preferred_width();

  virtual void set_label(const std::string &label);

  virtual void set_font(const sdlobj::Font &font);

 protected:
  inline std::string &label() {
    return label_;
  }

 private:
  std::string label_;
  bool valid_width_;
  unsigned width_;
};

#endif // GRAPH_LABELCONTROL_H_
