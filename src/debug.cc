#include <sstream>
#include <boost/format.hpp>
#include "logging.h"
#include "application.h"
#include "debug.h"

#if DEBUG_LEVEL != 0
namespace boost
{
  void assertion_failed_msg(const char *expr, const char *msg, const char *function, const char *file, long line) noexcept {
    // Assertions are sent for logging, then application is aborted.
    // We assume assertion fail is a critical error.
    LogCritical((boost::format("Assertion failed in %1%: %2% (%3%)") %
                 file % line % function).str().data());
    Application::instance().Abort();
  }

  void assertion_failed(const char *expr, const char *function, const char *file, long line) noexcept {
    assertion_failed_msg(expr, NULL, function, file, line);
  }
}
#endif
