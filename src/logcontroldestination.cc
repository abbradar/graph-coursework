#include <boost/format.hpp>
#include "logcontroldestination.h"

using namespace std;
using namespace logging;

LogControlDestination::LogControlDestination() {}

LogControlDestination::LogControlDestination(const std::shared_ptr<LogControl> &log_control)
  : log_control_(log_control) {}

void LogControlDestination::set_log_control(const std::shared_ptr<LogControl> &log_control) {
  log_control_ = log_control;
}

void LogControlDestination::WriteLog(const logging::LogMessageLevel level, const char *line) {
  if (log_control_) {
    string str = (boost::format("({1}) {2}") % LevelName(level)[0] % line).str();
    log_control_->AddItem(str);
  }
}
