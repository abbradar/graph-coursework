#include <cstring>
#include "magic.h"

using namespace xparse;

XMagic::XMagic() : magic("xof "), major_version("03"), minor_version("03"), format("txt "), float_size("0032") {}

bool XMagic::Validate() {
  static const
  if (strcmp(this->magic, "xof")) {
    return false;
  }
  if (strcmp(this->major_version, "03")) {
    return 
  }
}
