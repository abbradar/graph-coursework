#ifndef GRAPH_WINDOWSETTINGS_H_
#define GRAPH_WINDOWSETTINGS_H_

#include "window.h"
#include "common/settings.h"

namespace settings {

class WindowSettings : public SettingsBlock {
 public:
  WindowSettings(Window *window);
  virtual ~WindowSettings();

  inline Window *window() {
    return window_;
  }

  void set_window(Window *window);

  virtual const std::string name();
  virtual void operator <<(const YAML::Node &node);
 private:
  static const std::string kName;
  Window *window_;
};

}

#endif // GRAPH_WINDOWSETTINGS_H_
