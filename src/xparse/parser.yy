%{ /*** C/C++ Declarations ***/
#include <cstdio>
#include <string>
#include <list>
#include <boost/format.hpp>
#include "guid.h"
#include "xheader.h"
#include "xfile.h"

using namespace std;
%}

/*** yacc/bison Declarations ***/

/* Require bison 2.3 or later */
%require "2.3"

/* start symbol */
%start x-file

/* write out a header file containing the token defines */
%defines

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="xparse"

/* set the parser's class identifier */
%define "parser_class_name" "Parser"

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
%parse-param { Driver &driver }

/* verbose error messages */
%error-verbose

%union {
  XHeader *header;
  string *string_value;
  int int_value;
  double float_value;
  GUID *guid_value;

  XTemplateMember *template_member;
  XTemplate *template;
  XDataValue *data_value;
  XNode *node;
  list<int> *int_array;
  list<float> *float_array;
  list<XTemplateMember> *template_member_array;
  list<XNode> *node_array;
  XTemplateMember::BasicType base_type;
}

%token end-of-file 0
%token <header> header
%token <float_value> float-value
%token <int_value> integer-value
%token <string_value> string-value
%token <guid_value> guid-value
%token <string_value> type-name
%token array-keyword
%token template-keyword
%token triple-dot
%token <string_value> identifier

%type <template_member> type array-type
%type <int_value> dimension-size dimension
%type <int_array> dimension-list
%type <base_type> base-type
%type <template_member_array> member-list
%type <template> template-body

%destructor { delete $$; } header
%destructor { delete $$; } string-value base-type identifier
%destructor { delete $$; } guid-value

%{

#include "driver.h"
#include "lexer.h"

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer()->lex

%}

%% /*** Grammar Rules ***/

x-file : header data-list end-of-file

data-list : template-or-node | template-or-node data-list
template-or-node : template | data-node

template : template-keyword identifier '{' template-body '}' {
  $4->id() = identifier;
  driver.context->templates_.insert(pair<string, XTemplate>($4->id(), *$4));
  delete $4;
}

template-body : guid-value member-list restrictions {
  $$ = new XTemplate();
  $$->guid = $1;
  for (auto &i : *$2) {
    $$->members.insert(pair<string, XTemplateMember>(i.id(). i));
  }
  delete $2;
}

member-list : member {
  $$ = new list<XTemplateMember>();
  $$->push_front(*$1);
  delete $1;
} | member member-list {
  $$ = $2;
  $$->push_front(*$1);
  delete $1;
}

member : type identifier ';' {
  $$ = $1;
  $$->id() = $2;
}

type : base-type {
  $$ = new XTemplateMember(kBasic);
  $$->data().basic_type = $1;
} | array-type {
  $$ = $1;
} | identifier {
  error(yyloc, "Not supported");
}

array-type : array-keyword base-type dimension-list {
  $$ = new XTemplateMember(kArray);
  $$->data().array_type.basic_type = $2;
  $$->data().array_type.size->assign($3->begin(), $3->end());
  delete $3;
}

base-type : type-name {
  if (*$1 == "word" || *$1 == "dword" || *$1 == "ulonglong" || *$1 == "byte") {
    $$ = XTemplateMember::kInteger;
  } else if (*$1 == "float" || *$1 == "double") {
    $$ = XTemplateMember::kFloat;
  } else if (*$1 == "string" || *$1 == "cstring" || *$1 == "unicode") {
    $$ = XTemplateMember::kString;
  } else {
    error(yyloc, (boost::format("Type {1} is not supported") % $1).str()); 
  }
}

dimension-list : dimension {
  $$ = new list<int>();
  $$->push_front($1);
} | dimension dimension-list {
  $$ = $2;
  $$->push_front($1);
}

dimension : '[' dimension-size ']' {
  $$ = $2;
}

dimension-size : integer-value {
  $$ = $1;
} | identifier {
  error(yyloc, "Not implemented");
}

/* Not implemented */
restrictions : 
             | '[' triple-dot ']'
             | '[' restriction-list ']'
restriction-list : restriction | restriction ',' restriction-list
restriction : binary-keyword | identifier | identifier guid-value

data-node : identifier optional-identifier '{' optional-guid
            member-data-list nested-data-list '}'
optional-identifier : 
                    | identifier
optional-guid : 
              | guid-value
member-data-list : member-data | member-data member-data-list
member-data : data-value ';'
data-value : integer-value | float-value
           | string-value | member-data-list
           | array-value
array-value : integer-list | float-list
            | string-list | node-list
integer-list : integer-value | integer-value ',' integer-list
float-list : float-value | float-value ',' float-list
string-list : string-value | string-value ',' string-list
node-list : member-data-list | data-node ',' node-list
nested-data-list :
                 | nested-data nested-data-list
nested-data : data-node | data-reference
data-reference : '{' node-reference '}'
node-reference : identifier | guid-value | identifier guid-value

%%

