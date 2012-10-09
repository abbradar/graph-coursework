#include "stacktrace.h"

#ifdef __GNU_LIBRARY__
#include <sstream>
#include <execinfo.h>

const size_t kStackTraceSize = 24;
#endif

using namespace std;

string StackTrace() {
#ifdef __GNU_LIBRARY__
  void *stack[kStackTraceSize];
  size_t stack_size;
  stack_size = backtrace(stack, kStackTraceSize);

  char **trace_symbols = backtrace_symbols(stack, stack_size);
  stringstream ss;
  for (size_t i = 0; i < stack_size; ++i) {
    ss << trace_symbols[i] << endl;
  }
  free(trace_symbols);
  return ss.str();
#else
  return string("Stack trace is not avaliable\n");
#endif
}
