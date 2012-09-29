#include "guid.h"

const char * const kGUIDSize = 16;

inline char HexToInt(char hex) {
  if (hex >= '0' && hex <= '9') {
    return hex - '0';
  } else {
    switch (hex) {
      case 'a':
      case 'A': return 10;
      case 'b':
      case 'B': return 11;
      case 'c':
      case 'C': return 12;
      case 'd':
      case 'D': return 13;
      case 'e':
      case 'E': return 14;
      case 'f':
      case 'F': return 15;
      default: return (char)-1; // an error
    }
  }
}

void StringToGUID(const char *guid, char *out) {
  char curr = 0;
  int n = 0;
  char *cout = out;
  for (char *i = guid; n < kGUIDSize; ++i) {
    curr = HexToInt(*i);
    if (curr == (char)-1) continue;
    char next = HexToInt(*(++i));
    if (next == (char)-1) {
      // something is wrong with this GUID
      *cout = '\0';
      return;
    }
    *cout = curr << 4 + next;
    ++n;
  }
}
