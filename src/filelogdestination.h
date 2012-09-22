#ifndef GRAPH_FILELOGDESTINATION_H
#define GRAPH_FILELOGDESTINATION_H

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

#endif // GRAPH_FILELOGDESTINATION_H
