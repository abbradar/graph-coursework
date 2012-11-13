#ifndef GRAPH_LOGCONTROL_H_
#define GRAPH_LOGCONTROL_H_

#include <boost/circular_buffer.hpp>
#include "cachedtextcontrol.h"

class LogControl : public CachedTextControl {
 public:
  LogControl();

  void AddItem(const std::string &line);
  void Clear();

  inline int max_items() {
    return buffer_.capacity();
  }

  void set_max_items(const unsigned int max_items);

  virtual unsigned preferred_width();
  virtual unsigned preferred_height();

 protected:
  virtual void Repaint(sdlobj::Surface &surface);

 private:
  struct LogMessage {
    LogMessage(const std::string &message);
    ~LogMessage();

    std::string message;
    sdlobj::Surface *surface;
  };

  boost::circular_buffer<LogMessage> buffer_;

  void ClearRendered();
};

#endif // GRAPH_LOGCONTROL_H_
