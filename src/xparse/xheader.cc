#include <cstring>
#include "xheader.h"

namespace xparse {

XHeader::XHeader() : magic{'x','o','f',' '}, major_version{'0','3'}, minor_version{'0','3'},
  format{'t','x','t',' '}, float_size{'0','0','3','2'} {}

bool XHeader::Validate() {
  static const XHeader kReferenceHeader;

  if (memcmp(this->magic, kReferenceHeader.magic, sizeof(magic))) {
    return false;
  }
  if (memcmp(this->major_version, kReferenceHeader.major_version, sizeof(major_version))) {
    return false;
  }
  if (memcmp(this->minor_version, kReferenceHeader.minor_version, sizeof(minor_version))) {
    return false;
  }
  if (memcmp(this->format, kReferenceHeader.format, sizeof(format))) {
    return false;
  }
  return true;
}

}
