%{
#include <stdlib.h>
#include <stdio.h>
#include "guid.h"
#include "quoted.h"
#include "y.tab.h"
%}

%option outfile="xparse_lexer.c" header-file="xparse_header.h"
%option warn nodefault

%option noyywrap
%option case-insensitive
%option yylineno
%option never-interactive
%option bison-bridge
%option reentrant
%option nounistd

%%

^.*\n {
  yylval.string = yytext;
  return TMagic;
}

#.*\n |
//.*\n

-?[[:digit:]]+\.[[:digit:]]+ {
  yylval.float_value = atof(yytext);
  return TFloat;
}
-?[[:digit:]]+ {
  yylval.int_value = atoi(yytext);
  return TInteger;
}
\"([^"]|\\\")*\" {
  char *str = calloc(yyleng - 2, sizeof(char));
  QuotedToString(yytext, str);
  yylval.string = str;
  return TString;
}
[[:alnum:]]{8}-[[:alnum:]]{4}-[[:alnum:]]{4}-[[:alnum:]]{4}-[[:alnum:]]{12} {
  char *str = calloc(kGUIDSize, sizeof(char));
  StringToGUID(yytext, str);
  yylval.guid = str;
  return TGUID;
}

"[" |
"]" |
"{" |
"}" |
";" |
"," return *yytext;

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
  yylval.string = yytext;
  return TType;
}

template return TTemplate;
array return TArray;
binary return TBinary;
binary_resource return TBinaryResource;

[:alnum:]+ {
  yylval.string = yytext;
  return TIdentifier;
}

[\ \t\n]+ // return ' ';
. {
  char *error = malloc(40, sizeof(char));
  sprintf(error, "invalid character at line %i: %c", yylineno, *yytext);
  yyerror(error);
}

%%

void SetInput(FILE *input) {
  yyin = input;
}

