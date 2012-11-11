#include <memory>
#include "common/debug.h"
#include "common/exception.h"
#include "rwops.h"

using namespace std;
using namespace sdlobj;

int InputStreamSeek(SDL_RWops *context, int offset, int whence) {
  istream *in = (istream *)context->hidden.unknown.data1;
  ios_base::seekdir seekdir;
  switch (whence) {
    case RW_SEEK_SET:
      seekdir = ios_base::beg;
      break;
    case RW_SEEK_CUR:
      seekdir = ios_base::cur;
      break;
    case RW_SEEK_END:
      seekdir = ios_base::end;
      break;
    default:
      return -1;
  }
  in->seekg(offset, seekdir);
  if (in->fail()) {
    in->clear();
  } else if (in->bad()) {
    in->clear();
    return -1;
  }
  return in->tellg();
}

int InputStreamRead(SDL_RWops *context, void *ptr, int size, int maxnum) {
  istream *in = (istream *)context->hidden.unknown.data1;
  in->read((char *)ptr, size * maxnum);
  if (in->fail()) {
    in->clear();
  } else if (in->bad()) {
    in->clear();
    return -1;
  }
  return in->gcount() / size;
}

int InputStreamWrite(SDL_RWops *context, const void *ptr, int size, int maxnum) {
  AssertMsg(false, "Trying to write to read-only stream");
  return 0;
}

int InputStreamClose(SDL_RWops *context) {
  istream *in = (istream *)context->hidden.unknown.data1;
  delete in;
  SDL_FreeRW(context);
  return 0;
}

int OutputStreamSeek(SDL_RWops *context, int offset, int whence) {
  ostream *out = (ostream *)context->hidden.unknown.data1;
  ios_base::seekdir seekdir;
  switch (whence) {
    case RW_SEEK_SET:
      seekdir = ios_base::beg;
      break;
    case RW_SEEK_CUR:
      seekdir = ios_base::cur;
      break;
    case RW_SEEK_END:
      seekdir = ios_base::end;
      break;
    default:
      return -1;
  }
  out->seekp(offset, seekdir);
  if (out->fail()) {
    out->clear();
  } else if (out->bad()) {
    out->clear();
    return -1;
  }
  return out->tellp();
}

int OutputStreamRead(SDL_RWops *context, void *ptr, int size, int maxnum) {
  AssertMsg(false, "Trying to read write-only stream");
  return 0;
}

int OutputStreamWrite(SDL_RWops *context, const void *ptr, int size, int maxnum) {
  ostream *out = (ostream *)context->hidden.unknown.data1;
  out->write((const char *)ptr, size * maxnum);
  if (out->bad()) {
    return -1;
  }
  return maxnum;
}

int OutputStreamClose(SDL_RWops *context) {
  ostream *out = (ostream *)context->hidden.unknown.data1;
  delete out;
  SDL_FreeRW(context);
  return 0;
}

namespace sdlobj {

SDL_RWops *RWopsFromInputStream(std::unique_ptr<std::istream> &&in) {
  SDL_RWops *rwops = SDL_AllocRW();
  rwops->seek = &InputStreamSeek;
  rwops->read = &InputStreamRead;
  rwops->write = &InputStreamWrite;
  rwops->close = &InputStreamClose;
  rwops->hidden.unknown.data1 = in.get();
  in.release();
  return rwops;
}

SDL_RWops *RWopsFromOutputStream(std::unique_ptr<std::istream> &&out) {
  SDL_RWops *rwops = SDL_AllocRW();
  rwops->seek = &OutputStreamSeek;
  rwops->read = &OutputStreamRead;
  rwops->write = &OutputStreamWrite;
  rwops->close = &OutputStreamClose;
  rwops->hidden.unknown.data1 = out.get();
  out.release();
  return rwops;
}

}
