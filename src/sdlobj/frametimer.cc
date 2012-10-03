#include <stdexcept>
#include "frametimer.h"

using namespace std;
using namespace sdlobj;

#define MSECS_IN_SEC 1000

FrameTimer::FrameTimer(float fps) :
 first_tick_(true), measure_fps_(false), time_sum_(0),
 measure_ticks_(1), measured_ticks_(0), measured_fps_(fps_) {
  set_fps(fps);
}

void FrameTimer::set_fps(float fps) {
  if (fps <= 0) {
    throw runtime_error("FPS should be > 0");
  }
  fps_ = fps;
  tick_ = MSECS_IN_SEC / fps_;
  error_ = (float)MSECS_IN_SEC / fps_ - tick_;
  curr_error_ = 0;
}

void FrameTimer::set_measure_fps(bool measure_fps) {
  measure_fps_ = measure_fps;
  if (!measure_fps_) {
    measured_ticks_ = 0;
    measured_fps_ = fps_;
    time_sum_ = 0;
  }
}

void FrameTimer::set_measure_ticks(const unsigned int measure_ticks) {
  if (measure_ticks <= 0) {
    throw runtime_error("Measure ticks value should be > 0");
  }
  measure_ticks_ = measure_ticks;
}

bool FrameTimer::WaitFrame() {
  if (first_tick_) {
    first_tick_ = false;
  } else {
    ++measured_ticks_;
    if (measured_ticks_ >= measure_ticks_) {
      measured_fps_ = (float)MSECS_IN_SEC * measured_ticks_ / time_sum_;
      time_sum_ = 0;
      measured_ticks_ = 0;
    }
    int delay_time = 0;
    curr_error_ += error_;
    if (curr_error_ >= 1.0) {
      delay_time = 1;
      curr_error_ -= 1.0;
    }
    Uint32 curr_time = SDL_GetTicks();
    int frame_time = curr_time - prev_time_;
    delay_time += tick_ - frame_time;
    if (delay_time > 0) {
      time_sum_ += frame_time + delay_time;
      SDL_Delay(delay_time);
    } else {
      time_sum_ += frame_time;
    }
  }
  prev_time_ = SDL_GetTicks();
  return true;
}

void FrameTimer::Reset() {
  first_tick_ = true;
  curr_error_ = 0;

  measured_ticks_ = 0;
  measured_fps_ = fps_;
  time_sum_ = 0;
}
