#include "driver.h"
#include "xfile.h"

namespace xparse {

XFile::XFile() = default;

bool XFile::Parse(std::istream &in) {
  Driver driver(this);
  return driver.Parse(in);
}

}
