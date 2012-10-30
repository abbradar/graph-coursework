#include <cstring>
#include "xheader.h"

namespace xparse {

XHeader::XHeader() : magic{'x','o','f',' '}, major_version{'0','3'}, minor_version{'0','3'},
  format{'t','x','t',' '}, float_size{'0','0','3','2'} {}

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

}
