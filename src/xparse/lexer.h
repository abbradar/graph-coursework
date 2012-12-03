#ifndef XPARSE_SCANNER_H_
#define XPARSE_SCANNER_H_

// Flex expects the signature of yylex to be defined in the macro YY_DECL, and
// the C++ parser expects it to be declared. We can factor both as follows.

#ifndef YY_DECL
#define YY_DECL \
    xparse::Parser::token_type \
    xparse::Lexer::Lex(xparse::Parser::semantic_type *yylval, \
                       xparse::Parser::location_type *yylloc, \
                       xparse::Driver *driver)
#endif

#ifndef __FLEX_LEXER_H
#define yyFlexLexer XFlexLexer
#include "FlexLexer.h"
#undef yyFlexLexer
#endif

#include "parser.hh"

namespace xparse {

/** Scanner is a derived class to add some extra function to the scanner
 * class. Flex itself creates a class named yyFlexLexer, which is renamed using
 * macros to FlexLexer. However we change the context of the generated
 * yylex() function to be contained within the Scanner class. This is required
 * because the yylex() defined in FlexLexer has no parameters. */
class Lexer : public XFlexLexer {
 public:
  /** Create a new scanner object. The streams arg_yyin and arg_yyout default
   * to cin and cout, but that assignment is only made when initializing in
   * yylex(). */
  Lexer(std::istream *arg_yyin = nullptr,
        std::ostream *arg_yyout = nullptr);

  /** Required for virtual functions */
  virtual ~Lexer();

  /** This is the main lexing function. It is generated by flex according to
   * the macro declaration YY_DECL above. The generated bison parser then
   * calls this virtual function to fetch new tokens. */
  virtual Parser::token_type Lex(Parser::semantic_type *yylval,
                                 Parser::location_type *yylloc, Driver *driver);

  bool debug() const;

  /** Enable debug output (via arg_yyout) if compiled into the scanner. */
  void set_debug(bool debug);
};

} // namespace xparse

#endif // XPARSE_SCANNER_H_