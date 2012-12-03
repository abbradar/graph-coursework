#include <iostream>
#include <exception>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include "debug.h"
#include "logging.h"

using namespace std;
using namespace logging;
using namespace boost::posix_time;

const char *logging::LevelName(const LogMessageLevel level) {
  switch (level) {
    case kCritical:
      return "Critical";
    case kError:
      return "Error";
    case kWarning:
      return "Warning";
    case kNotice:
      return "Notice";
    case kDebug:
      return "Debug";
    default:
      AssertMsg(false, "Invalid log message level");
  }
  return nullptr;
}

Logger::LogDestination::~LogDestination() = default;

Logger::Logger() noexcept : write_to_stderr_(true), level_(kNotice),
 name_(NULL), time_facet_(new TimeFacet("%x %X")) {
  name_ = (char *)calloc(1, sizeof(char));
  name_[0] = '\0';
  time_format_.imbue(locale(time_format_.getloc(), time_facet_));
}

Logger::~Logger() {
  // boost bug; should not destroy this
  //delete time_facet_;
  free(name_);
}

void Logger::Log(LogMessageLevel level, const char * msg) noexcept {
  if (level > level_) return;

  if (write_to_stderr_) {
    boost::format a;
    time_format_ << second_clock::local_time();
    cerr << boost::format("%1% [%2%] %3%: %4%\n") % LevelName(level)[0] % time_format_.str() % name_ % msg;
    time_format_.str("");
  }

  for (auto i = destinations_.begin(); i != destinations_.end(); ++i) {
    try {
      (*i)->WriteLog(level, msg);
    } catch (exception &e) {
      destinations_.erase(i);
      if (destinations_.empty()) set_write_to_stderr(true);
      LogCritical((boost::format("Exception while logging: %1%") % e.what()).str().data());
      throw e;
    }
  }
}

void Logger::set_level(const LogMessageLevel level) noexcept {
  level_ = level;
}

void Logger::set_write_to_stderr(const bool write_to_stderr) noexcept {
  write_to_stderr_ = write_to_stderr;
}

void Logger::set_name(const char *name) noexcept {
  free(name_);
  name_ = strdup(name);
}

void Logger::set_time_facet(TimeFacet *time_facet) noexcept {
  delete time_facet_;
  time_facet_ = time_facet;
  time_format_.imbue(locale(time_format_.getloc(), time_facet_));
}
