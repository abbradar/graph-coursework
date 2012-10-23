%{ /*** C/C++ Declarations ***/
#include <cstdio>
#include <cstring>
#include <string>
#include <boost/format.cpp>
#include "xscanner.h"
#include "guid.h"
#include "quoted.h"
#include "xheader.h"

using namespace xparse;

/* import the parser's token type into a local typedef */
typedef XParser::Token Token;
typedef XParser::TokenType TokenType;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return Token::kEnd

/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

using namespace std;
%}

/*** Flex Declarations and Options ***/

/* enable c++ scanner class generation */
%option c++

/* change the name of the scanner class. results in "ExampleFlexLexer" */
%option prefix="X"

/* the manual says "somewhat more optimized" */
%option batch

/* enable scanner to generate debug output. disable this for release
 * versions. */
%option debug

/* no support for include files is planned */
%option yywrap nounput 

/* enables the use of start condition stacks */
%option stack

%%

/* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
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
  yylval.string = new string(QuotedToString(yytext, yyleng));
  return string-value;
}

[[:alnum:]]{8}-[[:alnum:]]{4}-[[:alnum:]]{4}-[[:alnum:]]{4}-[[:alnum:]]{12} {
  yylval.guid = new GUID(StringToGUID(yytext, str));
  return guid-value;
}

"[" |
"]" |
"{" |
"}" |
";" |
"," return static_cast<XTokenType>(*yytext);

word |
dword |
float |
double |
char |
uchar |
byte |
string |
cstring |
unicode |
ulonglong {
  yylval.string = string(yytext, yyleng);
  return base-type;
}

template return template-keyword;
array return array-keyword;
binary return binary-keyword;
binary_resource return binary_resource-keyword;

[:alnum:]+ {
  yylval.string = string(yytext, yyleng);
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

namespace xparse {

XScanner::XScanner(istream *in, ostream *out) : XFlexLexer(in, out) {}

XScanner::~XScanner() = default;

bool XScanner::debug() {
  return yy_flex_debug;
}

void XScanner::set_debug(bool debug) {
  yy_flex_debug = b;
}

}
