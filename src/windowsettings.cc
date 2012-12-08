#include <fstream>
#include "sdlobj/sdlimage.h"
#include "fontsettings.h"
#include "colorsettings.h"
#include "camerasettings.h"
#include "frametimersettings.h"
#include "windowsettings.h"

using namespace std;
using namespace sdlobj;

namespace settings {

WindowSettings::WindowSettings(Window *window) : window_(window) {}

WindowSettings::~WindowSettings() = default;

const std::string WindowSettings::kName = std::string("window");

const std::string WindowSettings::name() {
  return kName;
}

void WindowSettings::set_window(Window *window) {
  window_ = window;
}

void WindowSettings::operator <<(const YAML::Node &node) {
  {
    Font font;
    node["font"] >> font;
    window_->set_font(font);
    string pointer_name;
    node["pointer"] >> pointer_name;
    Surface pointer = SDLImage::Load(unique_ptr<istream>(new ifstream(pointer_name)));
    window_->set_pointer(pointer);
    Color font_color;
    node["font"]["color"] >> font_color;
    window_->set_font_color(font_color);
    float fps;
    node["fps"] >> fps;
    window_->set_fps(fps);
    bool show_fps;
    node["show-fps"] >> show_fps;
    window_->set_show_fps(show_fps);

    int show_fps_rate;
    node["show-fps-rate"] >> show_fps_rate;
    window_->set_show_fps_rate(show_fps_rate);
  }

  node["camera"] >> window_->context()->camera;
  
  {
    const YAML::Node &interface_node = node["interface"];
    myfloat move_speed;
    interface_node["move-speed"] >> move_speed;
    window_->set_move_speed(move_speed);
    myfloat rotation_speed;
    interface_node["rotation-speed"] >> rotation_speed;
    window_->set_rotation_speed(rotation_speed);
  }

  {
    const YAML::Node &video_mode_node = node["video-mode"];
    int width, height, bpp;
    video_mode_node["width"] >> width;
    video_mode_node["height"] >> height;
    video_mode_node["bpp"] >> bpp;
    window_->SetVideoMode(width, height, bpp);
  }
}

}
