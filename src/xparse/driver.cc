// $Id: driver.cc 39 2008-08-03 10:07:15Z tb $
/** \file driver.cc Implementation of the example::Driver class. */

#include <boost/format.hpp>
#include "common/logging.h"
#include "driver.h"
#include "lexer.h"

using namespace xparse;

Driver::Driver(XFile *context) : context_(context), trace_lexing_(false),
 trace_parsing_(false) { }

bool Driver::set_trace_lexing(bool trace_lexing) {
  trace_lexing_ = trace_lexing;
}

bool Driver::Parse(std::istream &in) {
  Scanner scanner(&in);
  scanner.set_debug(trace_lexing_);
  this->lexer = &scanner;

  Parser parser(*this);
  parser.set_debug_level(trace_parsing_);
  return (parser.parse() == 0);
}

void Driver::Error(const location &loc,
                   const std::string &msg) {
  LogError((boost::format(".x parse error: (%1%) %2%") % loc % msg).str().data());
}

void Driver::Error(const std::string &msg) {
  LogError((boost::format(".x parse error: %1%") % l % msg).str().data());
}

} // namespace example
