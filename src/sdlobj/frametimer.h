#ifndef SDLOBJ_FRAMETIMER_H_
#define SDLOBJ_FRAMETIMER_H_

#include <SDL_timer.h>

namespace sdlobj {

class FrameTimer {
 public:
  FrameTimer(float fps);

  inline float fps() {
    return fps_;
  }

  void set_fps(float fps);

  inline bool first_tick() {
    return first_tick_;
  }

  inline bool measure_fps() {
    return measure_fps_;
  }

  void set_measure_fps(bool measure_fps);

  inline int measure_ticks() {
    return measure_ticks_;
  }

  void set_measure_ticks(int measure_ticks);

  inline float measured_fps() {
    return measured_fps_;
  }

  bool WaitFrame();
  void Reset();

 private:
  float fps_;
  float error_;
  float curr_error_;
  int tick_;
  bool first_tick_;
  Uint32 prev_time_;
  bool measure_fps_;
  Uint32 time_sum_;
  int measure_ticks_;
  int measured_ticks_;
  float measured_fps_;
};

}

#endif // SDLOBJ_FRAMETIMER_H_
