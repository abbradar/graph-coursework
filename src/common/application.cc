#include <cstdlib>
#include <exception>
#include <string>
#include <iostream>
#include "logging.h"
#include "exception.h"
#include "stacktrace.h"
#include "application.h"

using namespace logging;
using namespace std;

#define InitSignalHandlers() ((void)(0))

#ifdef __GNU_LIBRARY__
#include <execinfo.h>
#ifndef STDERR_FILENO
#define STDERR_FILENO 2
#endif

const int kStackTraceSize = 24;

void PrintStackTrace() {
  void *trace[kStackTraceSize];
  size_t trace_size;
  trace_size = backtrace(trace, kStackTraceSize);

  // to stderr
  backtrace_symbols_fd(trace, trace_size, STDERR_FILENO);
}

void HandleTerminate() {
  // protection against recursive terminate()
  static bool is_terminating = false;
  if (is_terminating) {
    cerr << "Recursive OnTerminate() detected\n";
    exit(1);
  }
  is_terminating = true;
  LogCritical(StackTrace().data());
  exit(EXIT_FAILURE);
}

#ifdef __POSIX__
#include <signal.h>

void HandleSegfault(int sig) {
  LogCritical("Segfault caught");
  PrintStackTrace();
  exit(EXIT_FAILURE);
}

#undef InitSignalHandlers
#define InitSignalHandlers() { \
  signal(SIGSEGV, HandleSegfault); \
}
#endif
#else
#define HandleTerminate abort
#endif

Application::Application() = default;

int Application::Run(int argc, const char **argv) noexcept {
  if (!runnable_) {
    throw Exception("Runnable is not assigned.");
  }
  if (running_.exchange(true)) {
    throw Exception("Application is already started.");
  }
  set_terminate(&HandleTerminate);
  InitSignalHandlers();
  try {
    return runnable_->Run(argc, argv);
  } catch (Exception &e) {
    string s("Unhandled exception: ");
    s.append(e.what());
    LogCritical(s.c_str());
    LogCritical(e.stack_trace());
    Abort();
  }
  return EXIT_FAILURE;
}

void Application::Terminate(int error_code) noexcept {
  if (running_) {
    runnable_->Terminate(error_code);
  }
  exit(error_code);
}

void Application::Abort() noexcept {
  // TODO(abbradar) maybe better handling there, and maybe not
 abort();
}

void Application::set_runnable(Runnable *runnable) {
  if (running_) {
    throw Exception("Application is already started.");
  }
  runnable_.reset(runnable);
}

Application::~Application() = default;
