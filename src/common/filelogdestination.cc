#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include "filelogdestination.h"

using namespace std;
using namespace boost::posix_time;

FileLogDestination::FileLogDestination(const char *file_name) :
 stream_(file_name, ios_base::out | ios_base::app), time_facet_(new TimeFacet("%x %X")) {
  time_format_.imbue(locale(time_format_.getloc(), time_facet_));
}

FileLogDestination::~FileLogDestination() {
  stream_.close();
  delete time_facet_;
}

void FileLogDestination::WriteLog(logging::LogMessageLevel level, const char *line) {
  boost::format a;
  time_format_ << second_clock::local_time();
  stream_ << boost::format("%1% [%2%] %3%: %4%\n") % LevelName(level)[0] % time_format_.str() % logging::Logger::instance().name() % line;
  time_format_.str("");
  stream_.flush();
}

void FileLogDestination::set_time_facet(TimeFacet *time_facet) noexcept {
  delete time_facet_;
  time_facet_ = time_facet;
  time_format_.imbue(locale(time_format_.getloc(), time_facet_));
}
