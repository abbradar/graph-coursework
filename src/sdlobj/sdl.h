#ifndef SDLOBJ_SDL_H_
#define SDLOBJ_SDL_H_

#include <memory>
#include <SDL.h>
#include "common/singleton.h"
#include "surface.h"
#include "eventhandler.h"

namespace sdlobj {

class SDL : public Singleton<SDL> {
 public:
  void Init(const Uint32 flags);
  void Free();

  void SetVideoMode(unsigned int width, unsigned int height);
  void SetVideoMode(unsigned int width, unsigned int height, unsigned int bpp, Uint32 flags);

  inline Surface &surface() {
    return surface_;
  }

  /** Wait for events in the queue and process them */
  void WaitEvent();
  /** Immediately process events in the queue */
  bool PollEvent();

  void Flip();

  void UpdateRect(int x, int y, int width, int height);

  bool show_cursor();
  void set_show_cursor(const bool show);

  bool grab_input();
  void set_grab_input(bool grab);

  const char *caption();
  void set_caption(const char *caption);

  const char *icon_caption();
  void set_icon_caption(const char *caption);

  void set_icon(Surface &surface);

  void WarpMouse(const unsigned short x, const unsigned short y);

  bool translate_keysym();
  void set_translate_keysym(const bool translate_keysym);

  inline std::shared_ptr<EventHandler> &event_handler() {
    return event_handler_;
  }

  inline bool initialized() {
    return initialized_;
  }

  ~SDL();

 private:
  friend class Singleton<SDL>;

  SDL();

  bool initialized_;
  Surface surface_;
  std::shared_ptr<EventHandler> event_handler_;

  void ProcessEvent(const SDL_Event &event);
};

}

#endif // SDLOBJ_SDL_H_
