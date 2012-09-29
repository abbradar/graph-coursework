%{
#include "guid.h"
%}

%union {
  char *string;
  long int int_value;
  double float_value;
  char *guid;
}

%token <string> TMagic
%token <float_value> TFloat
%token <int_value> TInteger
%token <string> TString
%token <guid> TGUID
%token <string> TType
%token TTemplate
%token TArray
%token TBinary
%token TBinaryResource
%token <string> TIdentifier

%%



%%

