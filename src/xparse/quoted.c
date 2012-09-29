#include "quoted.h"

void QuotedToString(const char *str, char *out) {
  char *cin = str, *cout = out;
  do ++cin while (*cin != '"');;
  for (; *cin != '"'; ++cin, ++cout) {
    if (*cin == '\\') {
      ++cin;
      switch (*cin) {
        case 'n':
          *cout = '\n';
          break;
        case 't':
          *cout = '\t';
          break;
        case '\\':
          *cout = '\\';
          break;
        case '"':
          *cout = '"';
          break;
      }
    } else {
      *cout = *cin;
    }
  }
  *cout = '\0';
}
