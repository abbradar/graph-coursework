/*** yacc/bison Declarations ***/

/* Require bison 2.3 or later */
%require "2.3"

/* start symbol */
%start x-file

/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
%name-prefix="xparse"

/* set the parser's class identifier */
%define "parser_class_name" "Parser"

/* keep track of the current position within the input */
%locations

/* verbose error messages */
%error-verbose

/* write out a header file containing the token defines */
%defines

%code requires { /*** C/C++ Declarations ***/
#include <string>
#include <list>
#include "xparse/guid.h"
#include "xparse/xheader.h"
#include "xparse/xdata.h"
#include "xparse/xtemplate.h"
#include "xparse/driver.h"

struct XTemplateRestriction {
  xparse::XTemplate::RestrictionType restriction_type = xparse::XTemplate::kClosed;
  std::vector<xparse::XTemplateReference> *restrictions = nullptr;
};

}

/* The driver is passed by reference to the parser and to the scanner. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { xparse::Driver &driver }

%union {
  xparse::XHeader *header;
  std::string *string_value;
  int int_value;
  double float_value;
  xparse::GUID *guid_value;

  std::list<int> *int_array;
  std::list<float> *float_array;
  std::list<std::string> *string_array;

  xparse::XTemplate *template_item;
  std::list<xparse::XTemplateMember> *template_member_array;
  xparse::XTemplateMember *template_member;
  xparse::XTemplateMember::BasicType base_type;
  XTemplateRestriction *restriction;
  std::list<xparse::XTemplateReference> *template_reference_array;
  xparse::XTemplateReference *template_reference;

  xparse::XData *data_item;
  std::list<xparse::XDataValue> *data_value_array;
  xparse::XDataValue *data_value;
  std::list<xparse::XNestedData> *nested_data_array;
  xparse::XNestedData *nested_data;
  xparse::XDataReference *data_reference;
}

%token tEndOfFile 0
%token <header> tHeader
%token <float_value> tFloatValue
%token <int_value> tIntegerValue
%token <string_value> tStringValue
%token <guid_value> tGUIDValue
%token <string_value> tTypeName
%token tArrayKeyword
%token tTemplateKeyword
%token tBinaryKeyword
%token tBinaryResourceKeyword
%token tTripleDot
%token <string_value> tIdentifier


%type <template_item> template template-body
%type <template_member_array> member-list
%type <template_member> member member-description type
%type <base_type> base-type
%type <int_array> dimension-list
%type <int_value> dimension-size dimension
%type <restriction> restrictions
%type <template_reference_array> restriction-list
%type <template_reference> restriction

%type <data_item> data-node
%type <guid_value> optional-guid
%type <string_value> optional-identifier
%type <data_value_array> member-data-list
%type <data_value> member-data data-value
%type <nested_data_array> nested-data-list
%type <nested_data> nested-data
%type <data_reference> data-reference node-reference

%destructor { delete $$; } <header> <string_value> <guid_value>
%destructor { delete $$; } <int_array> <float_array> <string_array>

%destructor { delete $$; } <template_item> <template_member_array> <template_member>
%destructor { delete $$; } <restriction> <template_reference_array> <template_reference>

%destructor { delete $$; } <data_item> <data_value_array> <data_value>
%destructor { delete $$; } <nested_data_array> <nested_data> <data_reference>

%{
#include <cstdio>
#include <boost/format.hpp>
#include "xparse/driver.h"
#include "xparse/lexer.h"

using namespace std;
using namespace xparse;

/* this "connects" the bison parser in the driver to the flex scanner class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the driver context. */
#undef yylex
#define yylex driver.lexer->Lex
%}

%% /*** Grammar Rules ***/

x-file : tHeader data-list tEndOfFile {
  if (!$1->Validate())
    error(yyloc, "Invalid file header.");
  delete $1;
}

data-list : template-or-node | template-or-node data-list

template-or-node : template {
  driver.context()->templates().insert(pair<string, unique_ptr<XTemplate>>
    ($1->id, unique_ptr<XTemplate>($1)));
} | data-node {
  driver.context()->data_nodes().push_back(unique_ptr<XData>($1));
}

template : tTemplateKeyword tIdentifier '{' template-body '}' {
  $$ = $4;
  $$->id = move(*$2);
  delete $2;
}

template-body : tGUIDValue member-list restrictions {
  $$ = new XTemplate();
  $$->guid = move(*$1);
  delete $1;
  $$->members.assign($2->begin(), $2->end());
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

member-description : type tIdentifier {
  $$ = $1;
  $$->id() = std::move(*$2);
  delete $2;
} | tArrayKeyword type tIdentifier dimension-list {
  $$ = new XTemplateMember(XTemplateMember::kArray, $2->basic_type());
  if ($$->basic_type() == XTemplateMember::kNode) {
    *($$->template_reference()) = *($2->template_reference());
  }
  delete $2;
  $$->id() = std::move(*$3);
  delete $3;
  $$->array_size()->assign($4->begin(), $4->end());
  delete $4;
}

type : base-type {
  $$ = new XTemplateMember(XTemplateMember::kBasic, $1);
} | tIdentifier {
  $$ = new XTemplateMember(XTemplateMember::kBasic, XTemplateMember::kNode);
  $$->template_reference()->id = move(*$1);
  delete $1;
  if (!$$->template_reference()->Resolve(driver.context())) {
    error(yyloc, "Cannot resolve nested template type");
  }
}

base-type : tTypeName {
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

dimension-size : tIntegerValue
               | tIdentifier {
  delete $1;
  error(yyloc, "Not implemented");
}

restrictions : {
  $$ = new XTemplateRestriction();
  $$->restriction_type = XTemplate::kClosed;
} | '[' tTripleDot ']' {
  $$ = new XTemplateRestriction();
  $$->restriction_type = XTemplate::kOpened;
} | '[' restriction-list ']' {
  $$ = new XTemplateRestriction();
  $$->restriction_type = XTemplate::kRestricted;
  $$->restrictions = new vector<XTemplateReference>();
  $$->restrictions->assign($2->begin(), $2->end());
  delete $2;
}

restriction-list : restriction {
  $$ = new list<XTemplateReference>();
  $$->push_front(move(*$1));
  delete $1;
} | restriction ',' restriction-list {
  $$ = $3;
  $$->push_front(move(*$1));
  delete $1;
}

restriction : tBinaryKeyword {
  error(yyloc, "Not implemented");
  $$ = new XTemplateReference();
} | tIdentifier {
  $$ = new XTemplateReference();
  $$->id = move(*$1);
  delete $1;
  if (!$$->Resolve(driver.context())) {
    error(yyloc, "Cannot resolve template reference for restriction");
  }
} | tIdentifier tGUIDValue {
  $$ = new XTemplateReference();
  $$->id = *$1;
  delete $1;
  $$->guid = move(*$2);
  delete $2;
  if (!$$->Resolve(driver.context())) {
    error(yyloc, "Cannot resolve template reference");
  }
}

data-node : tIdentifier optional-identifier '{' optional-guid
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
} | tIdentifier

optional-guid : {
  $$ = new GUID();
} | tGUIDValue

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

data-value : tIntegerValue {
  $$ = new XDataValue(XDataValue::kInteger);
  $$->data().int_value = $1;
} | tFloatValue {
  $$ = new XDataValue(XDataValue::kFloat);
  $$->data().float_value = $1;
} | tStringValue {
  $$ = new XDataValue(XDataValue::kString);
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
  $$ = new XNestedData(XNestedData::kNode);
  $$->data().node = $1;
} | data-reference {
  $$ = new XNestedData(XNestedData::kNodeReference);
  $$->data().reference = $1;
}

data-reference : '{' node-reference '}' {
  $$ = $2;
  if (!$$->Resolve(driver.context())) {
    error(yyloc, "Cannot resolve data reference");
  }
}

node-reference : tIdentifier {
  $$ = new XDataReference();
  $$->id = move(*$1);
  delete $1;
} | tGUIDValue {
  $$ = new XDataReference();
  $$->guid = move(*$1);
  delete $1;
} | tIdentifier tGUIDValue {
  $$ = new XDataReference();
  $$->id = move(*$1);
  delete $1;
  $$->guid = move(*$2);
  delete $2;
}

%%

void xparse::Parser::error(const Parser::location_type& loc,
                            const std::string& msg) {
  driver.Error(loc, msg);
}
