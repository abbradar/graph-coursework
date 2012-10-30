%{ /*** C/C++ Declarations ***/
#include <cstring>
#include <string>
#include <iostream>
#include <boost/format.hpp>
#include "common/debug.h"
#include "xparse/lexer.h"
#include "xparse/guid.h"
#include "xparse/quoted.h"
#include "xparse/xheader.h"

using namespace xparse;
using namespace std;

/* import the parser's token type into a local typedef */
typedef Parser::token token;
typedef Parser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token_type::tEndOfFile;

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H
%}

/*** Flex Declarations and Options ***/

/* enable c++ scanner class generation */
%option c++

/* the manual says "somewhat more optimized" */
%option batch

/* no support for include files is planned */
%option noyywrap

/* enables the use of start condition stacks */
%option stack

/* change the name of lexer class */
%option prefix="X"

/** The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. **/
%{
#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}

^.{16} {
  XHeader *header = new XHeader();
  memcpy(header, yytext, sizeof(XHeader));
  yylval->header = header;
  return token::tHeader;
}

#.* |
\/\/.* {
  yylloc->step();
}

-?[[:digit:]]+\.[[:digit:]]+ {
  yylval->float_value = atof(yytext);
  return token::tFloatValue;
}
-?[[:digit:]]+ {
  yylval->int_value = atoi(yytext);
  return token::tIntegerValue;
}
\"([^"]|\\\")*\" {
  yylval->string_value = new string(QuotedToString(yytext, yyleng));
  return token::tStringValue;
}

[[:alnum:]]{8}-[[:alnum:]]{4}-[[:alnum:]]{4}-[[:alnum:]]{4}-[[:alnum:]]{12} {
  yylval->guid_value = new GUID(yytext);
  return token::tGUIDValue;
}

\.\.\. return token::tTripleDot;

"[" |
"]" |
"{" |
"}" |
";" |
"," return static_cast<token_type>(*yytext);

"word" |
"dword" |
"float" |
"double" |
"char" |
"uchar" |
"byte" |
"string" |
"cstring" |
"unicode" |
"ulonglong" {
  yylval->string_value = new string(yytext, yyleng);
  return token::tTypeName;
}

"template" return token::tTemplateKeyword;
"array" return token::tArrayKeyword;
"binary" return token::tBinaryKeyword;
"binary_resource" return token::tBinaryResourceKeyword;

[:alnum:]+ {
  yylval->string_value = new string(yytext, yyleng);
  return token::tIdentifier;
}

[ \t\r] {
  yylloc->step();
}

\n {
  yylloc->lines(yyleng);
  // return end-of-line;
}

. {
  string error = (boost::format("Invalid characher at line %1%: %2%") % yylineno % *yytext).str();
  YY_FATAL_ERROR(error.data());
}

%%

Lexer::Lexer(istream *in, ostream *out) : XFlexLexer(in, out) {}

Lexer::~Lexer() = default;

bool Lexer::debug() const {
  return yy_flex_debug;
}

void Lexer::set_debug(bool debug) {
  yy_flex_debug = debug;
}

/* This implementation of ExampleFlexLexer::yylex() is required to fill the
* vtable of the class ExampleFlexLexer. We define the scanner's main yylex
* function via YY_DECL to reside in the Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int XFlexLexer::yylex() {
  Assert(false);
  return 0;
}
