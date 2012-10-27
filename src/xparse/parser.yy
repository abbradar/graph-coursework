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

  list<int> *int_array;
  list<float> *float_array;
  list<string> *string_array;

  XTemplate *template;
  list<XTemplateMember> *template_member_array;
  XTemplateMember *template_member;
  XTemplateMember::BasicType base_type;

  XNode *node;
  list<XDataValue> *data_value_array;
  XDataValue *data_value;
  list<XNode> *node_array;
  XDataReference *data_reference;
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

%type <template> template template-body
%type <template_member_array> member-list
%type <template_member> type array-type
%type <base_type> base-type
%type <int_array> dimension-list
%type <int_value> dimension-size dimension

%type <node> data-node
%type <guid_value> optional-guid
%type <string_value> optional-identifier
%type <data_value_array> member-data-list
%type <data_value> member-data data-value data-reference
%type <int_array> integer-list
%type <float_array> float-list
%type <string_array> string-list
%type <node_array> node-list
%type <data_reference> node-reference

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
template-or-node : template {
  driver.context()->templates().insert(pair<string, XTemplate>($1->id(), move(*$1)));
  delete $1;
} | data-node {
  driver.context()->data_nodes().push_back(move(*$1));
  delete $1;
}

template : template-keyword identifier '{' template-body '}' {
  $$ = $4;
  $$->id() = move(*$2);
  delete $2;
}

template-body : guid-value member-list restrictions {
  $$ = new XTemplate();
  $$->guid = move(*$1);
  delete $1;
  for (auto &i : *$2) {
    $$->members.insert(pair<string, XTemplateMember>(i.id(), move(i)));
  }
  delete $2;
}

member-list : member {
  $$ = new list<XTemplateMember>();
  $$->push_front(move(*$1));
  delete $1;
} | member member-list {
  $$ = $2;
  $$->push_front(move(*$1));
  delete $1;
}

member : type identifier ';' {
  $$ = $1;
  $$->id() = move(*$2);
  delete $2;
}

type : base-type {
  $$ = new XTemplateMember(kBasic);
  $$->data().basic_type = $1;
} | array-type
  | identifier {
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
  delete $1;
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

dimension-size : integer-value
               | identifier {
  error(yyloc, "Not implemented");
}

/* Not implemented */
restrictions : 
             | '[' triple-dot ']'
             | '[' restriction-list ']'
restriction-list : restriction | restriction ',' restriction-list
restriction : binary-keyword | identifier | identifier guid-value

data-node : identifier optional-identifier '{' optional-guid
            member-data-list '}' {
  $$ = new XNode();
  node.template_id = move(*$1);
  delete $1;
  node.id = move(*$2);
  delete $2;
  node.guid = move(*$4);
  delete $4;
  node.data.assign($5->begin(), $5->end());
  delete $5;
}

optional-identifier : {
  $$ = new string();
} | identifier

optional-guid : {
  $$ = new GUID();
} | guid-value

member-data-list : member-data {
  $$ = new list<XDataValue>();
  $$->push_front(move(*$1));
  delete $1;
} | member-data member-data-list {
  $$ = $2;
  $$->push_front(move(*$1));
  delete $1;
}

member-data : data-value ';'

data-value : integer-value {
  $$ = new XDataValue(kInteger);
  $$->data().int_value = $1;
} | float-value {
  $$ = new XDataValue(kFloat);
  $$->data().float_value = $1;
} | string-value {
  $$ = new XDataValue(kString);
  *($$->data().string_value) = move(*$1);
  delete $1;
} | array-value | data-node | data-reference

array-value : integer-list {
  $$ = new XDataValue(kIntegerArray);
  $$->data().int_array->assign($1->begin(), $1->end());
  delete $1;
} | float-list {
  $$ = new XDataValue(kFloatArray);
  $$->data().int_array->assign($1->begin(), $1->end());
  delete $1;
} | string-list {
  $$ = new XDataValue(kStringArray);
  $$->data().float_array->assign($1->begin(), $1->end());
  delete $1;
} | node-list {
  $$ = new XDataValue(kNodeArray);
  $$->data().node_array->assign($1->begin(), $1->end());
  delete $1;
}

integer-list : integer-value {
  $$ = new list<int>();
  $$->push_front($1);
} | integer-value ',' integer-list {
  $$ = $3;
  $$->push_front($1);
}

float-list : float-value {
  $$ = new list<float>();
  $$->push_front($1);
} | float-value ',' float-list {
  $$ = $3;
  $$->push_front($1);
}

string-list : string-value {
  $$ = new list<string>();
  $$->push_front(move(*$1));
  delete $1;
} | string-value ',' string-list {
  $$ = $3;
  $$->push_front(move(*$1));
  delete $1;
}

node-list : data-node {
  $$ = new list<XNode>();
  $$->push_front(move(*$1));
  delete $1;
} | data-node ',' node-list {
  $$ = $3;
  $$->push_front(move(*$1));
  delete $1;
}

data-reference : '{' node-reference '}' {
  $$ = new XDataValue(kNodeReference);
  *($$->data().node_reference) = move(*$2);
  delete $2;
}

node-reference : identifier {
  $$ = new XDataReference();
  $$->id = move(*$1);
  delete $1;
} | guid-value {
  $$ = new XDataReference();
  $$->guid = move(*$1);
  delete $1;
} | identifier guid-value {
  $$ = new XDataReference();
  $$->id = move(*$1);
  delete $1;
  $$->guid = move(*$2);
  delete $2;
}

%%

