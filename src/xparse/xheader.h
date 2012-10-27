#ifndef XPARSE_XHEADER_H_
#define XPARSE_XHEADER_H_

namespace xparse {

struct XHeader {
 public:
  XHeader();
  
  char magic[4];
  char major_version[2];
  char minor_version[2];
  char format[4];
  char float_size[4];

  bool Validate();
};

}

#endif // XPARSE_XHEADER_H_
