// $Id: driver.cc 39 2008-08-03 10:07:15Z tb $
/** \file driver.cc Implementation of the example::Driver class. */

#include <boost/format.hpp>
#include "common/logging.h"
#include "driver.h"
#include "lexer.h"

namespace xparse {

Driver::Driver(XFile *context) : context_(context), trace_lexing_(false),
  trace_parsing_(false), error_(false) { }

void Driver::set_trace_lexing(bool trace_lexing) {
  trace_lexing_ = trace_lexing;
}

void Driver::set_trace_parsing(bool trace_parsing) {
  trace_parsing_ = trace_parsing;
}

bool Driver::Parse(std::istream &in) {
  Lexer lexer(&in);
  lexer.set_debug(trace_lexing_);
  this->lexer = &lexer;

  Parser parser(this);
#if DEBUG_LEVEL >= 4
  parser.set_debug_level(trace_parsing_);
#endif
  error_ = false;
  if (parser.parse()) return false;
  return !error_;
}

void Driver::Error(const location &loc,
                   const std::string &msg) {
  LogError((boost::format(".x parse error: (%1%) %2%") % loc % msg).str().data());
  error_ = true;
}

void Driver::Error(const std::string &msg) {
  LogError((boost::format(".x parse error: %1%") % msg).str().data());
  error_ = true;
}

}
