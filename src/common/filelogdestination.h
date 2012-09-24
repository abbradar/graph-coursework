#ifndef COMMON_FILELOGDESTINATION_H_
#define COMMON_FILELOGDESTINATION_H_

#include <fstream>
#include <sstream>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include "logging.h"

class FileLogDestination : public logging::LogDestination {
 public:
  typedef boost::posix_time::time_facet TimeFacet;

  explicit FileLogDestination(const char *file_name);
  ~FileLogDestination();

  void WriteLog(logging::LogMessageLevel level, const char *line);

  inline const TimeFacet &time_facet() const noexcept {
    return *time_facet_;
  }

  void set_time_facet(TimeFacet *time_facet) noexcept;

 private:
  std::ofstream stream_;
  TimeFacet *time_facet_;
  std::stringstream time_format_;
};

#endif // COMMON_FILELOGDESTINATION_H_
