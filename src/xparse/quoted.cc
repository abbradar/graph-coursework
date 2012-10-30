#include <sstream>
#include "quoted.h"

using namespace std;

namespace xparse {

string QuotedToString(const char *str, size_t size) {
  stringstream ss;
  for (size_t i = 1; i < size - 1;) {
    if (str[i] == '\\') {
      ++i;
      switch (str[i]) {
        case 'n':
          ss << '\n';
          break;
        case 't':
          ss << '\t';
          break;
        case '\\':
          ss << '\\';
          break;
        case '"':
          ss << '"';
          break;
      }
    } else {
      ss << str[i];
    }
  }
  return ss.str();
}

}
