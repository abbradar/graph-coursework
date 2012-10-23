%{ /*** C/C++ Declarations ***/
#include <cstdio>
#include <string>
%}

/*** yacc/bison Declarations ***/

/* Require bison 2.3 or later */
%require "2.3"

/* add debug output code to generated parser. disable this for release
 * versions. */
%debug

/* start symbol is named "start" */
%start start

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="example"

/* set the parser's class identifier */
%define "parser_class_name" "XParser"

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &driver.streamname;
};

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class Driver& driver }

/* verbose error messages */
%error-verbose

%union {
  XHeader *header;
  string *string;
  int int_value;
  double float_value;
  GUID *guid;
}

%token <header> kXHeader
%token <float_value> kFloat
%token <int_value> kInteger
%token <string> kString
%token <guid> kGUID
%token <string> kType
%token <string> kIdentifier

%%



%%

