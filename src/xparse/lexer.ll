%{ /*** C/C++ Declarations ***/
#include <cstring>
#include <string>
#include <iostream>
#include "common/debug.h"
#include "lexer.h"
#include "guid.h"
#include "quoted.h"
#include "xheader.h"

using namespace xparse;
using namespace std;

/* import the parser's token type into a local typedef */
typedef Parser::token token;
typedef Parser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token_type::end-of-file

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
%option yywrap nounput

/* enables the use of start condition stacks */
%option stack

%%

/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
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
  ylval->header = header;
  return header;
}

#.* |
//.* {
  yylloc->step();
}

-?[[:digit:]]+\.[[:digit:]]+ {
  yylval.float_value = atof(yytext);
  return float-value;
}
-?[[:digit:]]+ {
  yylval.int_value = atoi(yytext);
  return integer-value;
}
\"([^"]|\\\")*\" {
  yylval.string_value = new string(QuotedToString(yytext, yyleng));
  return string-value;
}

[[:alnum:]]{8}-[[:alnum:]]{4}-[[:alnum:]]{4}-[[:alnum:]]{4}-[[:alnum:]]{12} {
  yylval.guid_value = new GUID(StringToGUID(yytext, str));
  return guid-value;
}

\.\.\. {
  return triple-dot
}

"[" |
"]" |
"{" |
"}" |
";" |
"," return static_cast<TokenType>(*yytext);

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
  yylval.string_value = string(yytext, yyleng);
  return type-name;
}

"template" return template-keyword;
"array" return array-keyword;
"binary" return binary-keyword;
"binary_resource" return binary_resource-keyword;

[:alnum:]+ {
  yylval.string_value = string(yytext, yyleng);
  return identifier;
}

[ \t\r] {
  yylloc->step();
}

\n {
  yylloc->lines(yyleng);
  yylloc->step();
}

. {
  string error = (boost::format("Invalid characher at line %1%: %2%") % yylineno, *yytext).str();
  yyerror(error);
}

%%

Lexer::Lexer(istream *in, ostream *out) : FlexLexer(in, out) {}

Lexer::~Lexer() = default;

bool Scanner::debug() {
  return yy_flex_debug;
}

void Scanner::set_debug(bool debug) {
  yy_flex_debug = debug;
}

/* This implementation of ExampleFlexLexer::yylex() is required to fill the
* vtable of the class ExampleFlexLexer. We define the scanner's main yylex
* function via YY_DECL to reside in the Scanner class instead. */

#ifdef yylex
#undef yylex
#endif

int FlexLexer::yylex() {
  Assert(false);
  return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
* checks the yywrap() function. If yywrap() returns false (zero), then it is
* assumed that the function has gone ahead and set up `yyin' to point to
* another input file, and scanning continues. If it returns true (non-zero),
* then the scanner terminates, returning 0 to its caller. */

int FlexLexer::yywrap() {
  return 1;
}
