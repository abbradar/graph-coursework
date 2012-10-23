#include <cstring>
#include "xmagic.h"

using namespace xparse;

XHeader::XHeader() : magic("xof "), major_version("03"), minor_version("03"), format("txt "), float_size("0032") {}

bool XHeader::Validate() {
  static const XHeader kReferenceHeader;

  if (strncmp(this->magic, kReferenceHeader.magic, sizeof(kReferenceHeader.magic))) {
    return false;
  }
  if (strncmp(this->major_version, kReferenceHeader.major_version, sizeof(kReferenceHeader.major_version))) {
    return false;
  }
  if (strncmp(this->minor_version, kReferenceHeader.minor_version, sizeof(kReferenceHeader.minor_version))) {
    return false;
  }
  if (strncmp(this->format, kReferenceHeader.format, sizeof(kReferenceHeader.format))) {
    return false;
  }
  return true;
}
