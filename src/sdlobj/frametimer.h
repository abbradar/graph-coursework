#ifndef SDLOBJ_FRAMETIMER_H_
#define SDLOBJ_FRAMETIMER_H_

#include <SDL_timer.h>

namespace sdlobj {

class FrameTimer {
 public:
  FrameTimer();
  FrameTimer(const float fps);

  inline float fps() {
    return fps_;
  }

  void set_fps(const float fps);

  inline bool first_tick() {
    return first_tick_;
  }

  inline bool measure_fps() {
    return measure_fps_;
  }

  void set_measure_fps(const bool measure_fps);

  inline unsigned int measure_ticks() {
    return measure_ticks_;
  }

  void set_measure_ticks(const unsigned int measure_ticks);

  inline float measured_fps() {
    return measured_fps_;
  }

  bool WaitFrame();
  void Reset();

 private:
  float fps_;
  float error_;
  float curr_error_;
  unsigned int tick_;
  bool first_tick_;
  Uint32 prev_time_;
  bool measure_fps_;
  Uint32 time_sum_;
  unsigned int measure_ticks_;
  unsigned int measured_ticks_;
  float measured_fps_;
};

}

#endif // SDLOBJ_FRAMETIMER_H_
