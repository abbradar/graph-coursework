#ifndef GRAPH_WINDOWLOGDESTINATION_H_
#define GRAPH_WINDOWLOGDESTINATION_H_

#include <memory>
#include "common/logging.h"
#include "logcontrol.h"

class LogControlDestination : public logging::Logger::LogDestination {
 public:
  LogControlDestination();
  LogControlDestination(const std::shared_ptr<LogControl> &log_control);

  inline const std::shared_ptr<LogControl> &log_control() {
    return log_control_;
  }

  void set_log_control(const std::shared_ptr<LogControl> &log_control);

  void WriteLog(const logging::LogMessageLevel level, const char *line);

 private:
  std::shared_ptr<LogControl> log_control_;
};

#endif // WINDOWLOGDESTINATION_H
