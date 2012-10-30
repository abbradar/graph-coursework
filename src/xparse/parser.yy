%{ /*** C/C++ Declarations ***/
#include <cstdio>
#include <string>
#include <list>
#include <boost/format.hpp>
#include "guid.h"
#include "xheader.h"
#include "xdata.h"

using namespace std;

struct XTemplateRestriction {
  XTemplate::RestrictionType restriction_type = kClosed;
  vector<XTemplateReference> *restrictions = nullptr;
}

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
  XTemplateRestriction *restriction;
  list<XTemplateReference> *template_reference_array;
  XTemplateReference *template_reference;

  XData *node;
  list<XDataValue> *data_value_array;
  XDataValue *data_value;
  list<XNestedData> *nested_data_array;
  XNestedData *nested_data;
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
%token binary-keyword
%token triple-dot
%token <string_value> identifier

%type <template> template template-body
%type <template_member_array> member-list
%type <template_member> member type array-type
%type <base_type> base-type
%type <int_array> dimension-list
%type <int_value> dimension-size dimension
%type <restriction> restrictions
%type <template_reference_array> restriction-list
%type <template_reference> restriction

%type <node> data-node
%type <guid_value> optional-guid
%type <string_value> optional-identifier
%type <data_value_array> member-data-list
%type <data_value> member-data data-value
%type <nested_data_array> nested-data-list
%type <nested_data> nested-data
%type <data_reference> data-reference node-reference

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

x-file : header data-list end-of-file {
  delete $1;
}

data-list : template-or-node | template-or-node data-list

template-or-node : template {
  driver.context()->templates().insert(pair<string, unique_ptr<XTemplate>>
    ($1->id(), unique_ptr<XTemplate>($1)));
} | data-node {
  driver.context()->data_nodes().push_back(unique_ptr<XData>($1));
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
    $$->members.insert(pair<XTemplateMember>(i.id(), move(i)));
  }
  delete $2;
  $$->restriction_type = $3->restriction_type;
  $$->restrictions.reset($3->restrictions);
  delete $3;
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

member : member-description ';'

member-description : type identifier {
  $$ = $1;
  $$->id() = std::move(*$2);
  delete $2;
} | array-keyword type identifier dimension-list {
  $$ = new XTemplateMember(kArray, $2->basic_type());
  if ($$->basic_type() == kTemplate) {
    *($$->template_reference()) = *($2->template_reference());
  }
  delete $2;
  $$->id() = std::move(*$3);
  delete $3;
  $$->array_size()->assign($4->begin(), $4->end());
  delete $4;
}

type : base-type {
  $$ = new XTemplateMember(kBasic, $1);
} | identifier {
  $$ = new XTemplateMember(kBasic, kTemplate);
  $$->template_reference()->id = move(*$1);
  delete $1;
  if (!$$->template_reference()->Resolve(driver.context())) {
    error(yyloc, "Cannot resolve nested template type");
  }
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
  delete $1;
  error(yyloc, "Not implemented");
}

restrictions : 
             | '[' triple-dot ']' {
  $$ = new XTemplateRestriction();
  $$->restriction_type = kOpened;
} | '[' restriction-list ']' {
  $$ = new XTemplateRestriction();
  $$->restriction_type = kRestricted;
  $$->restrictions = new vector<XTemplateReference>();
  $$->restrictions.assign($2->begin(), $2->end());
  delete $2;
}

restriction-list : restriction {
  $$ = list<XTemplateReference>();
  $$->push_front(move(*$1));
  delete $1;
} | restriction ',' restriction-list {
  $$ = $3;
  $$->push_front(move(*$1));
  delete $1;
}

restriction : binary-keyword {
  error(yyloc, "Not implemented");
} | identifier {
  $$ = new XTemplateReference();
  $$->id = move(*$1);
  delete $1;
  if (!$$->Resolve(driver.context())) {
    error(yyloc, "Cannot resolve template reference for restriction");
  }
} | identifier guid-value {
  $$ = new XTemplateReference();
  $$->id = *$1;
  delete $1;
  $$->guid = move(*$2);
  delete $2;
  if (!$$->Resolve(driver.context())) {
    error(yyloc, "Cannot resolve template reference");
  }
}

data-node : identifier optional-identifier '{' optional-guid
            member-data-list nested-data-list '}' {
  $$ = new XData();
  $$->template_id = move(*$1);
  delete $1;
  $$->id = move(*$2);
  delete $2;
  $$->guid = move(*$4);
  delete $4;
  $$->data.assign($5->begin(), $5->end());
  delete $5;
  $$->nested_data.assign($6->begin(), $6->end());
  delete $6;
  if (!$$->Validate(driver.context())) {
    error(yyloc, "Data cannot be validated by template");
  }
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

/* Dirty grammar which ignores some file structure; it will be built by template */
member-data : data-value maybe-comma semicolon-list

maybe-comma : | ','
semicolon-list : ';' | ';' semicolon-list

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
}

nested-data-list : nested-data {
  $$ = new list<XNestedData>();
  $$->push_front(move(*$1));
  delete $1;
} | nested-data nested-data-list {
  $$ = $2;
  $$->push_front(move(*$1));
  delete $1;
}

nested-data : data-node {
  $$ = new XNestedData(kNode);
  $$->data().node = $1;
} | data-reference {
  $$ = new XNestedData(kNodeReference);
  $$->data().reference = $1;
}

data-reference : '{' node-reference '}' {
  $$ = $2;
  if (!$$->Resolve(driver.context())) {
    error(yyloc, "Cannot resolve data reference");
  }
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

