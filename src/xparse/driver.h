#ifndef XPARSE_DRIVER_H_
#define XPARSE_DRIVER_H_

#include <string>
#include <istream>

namespace xparse {

class XFile;
class Parser;
class location;

/** The Driver class brings together all components. It creates an instance of
 * the Parser and Scanner classes and connects them. Then the input stream is
 * fed into the scanner object and the parser gets it's token
 * sequence. Furthermore the driver object is available in the grammar rules as
 * a parameter. Therefore the driver class contains a reference to the
 * structure into which the parsed data is saved. */
class Driver {
 public:
  /// construct a new parser driver context
  Driver(XFile *context);

  inline bool trace_lexing() {
    return trace_lexing_;
  }

  bool set_trace_lexing(bool trace_lexing);

  inline bool trace_parsing() {
    return trace_parsing_;
  }

  bool set_trace_parsing(bool trace_parsing);

  inline XFile *context() {
    return context_;
  }

  void set_context(XFile *context);

  bool Parse(std::istream &in);

 private:
  friend class Parser;

  // To demonstrate pure handling of parse errors, instead of
  // simply dumping them on the standard error output, we will pass
  // them to the driver using the following two member functions.

  /** Error handling with associated line number. This can be modified to
   * output the error e.g. to a dialog box. */
  void Error(const location &loc, const std::string &msg);

  /** General error handling. This can be modified to output the error
   * e.g. to a dialog box. */
  void Error(const std::string &msg);

  XFile *context_;
  bool trace_lexing_;
  bool trace_parsing_;
};

} // namespace xparse

#endif // EXAMPLE_DRIVER_H
