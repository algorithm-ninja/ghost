#include "net/packets.h"
#include <stddef.h>
#include <string.h>
#include <type_traits>
#include <unistd.h>

namespace net {

bool ClientHello::Write(int fd) const {
  using T = std::decay_t<decltype(*this)>;
  static_assert(std::is_standard_layout_v<T>);
  uint8_t flen = strlen((char *)filename);
  return write(fd, this, offsetof(T, filename) + flen) == -1;
}

bool ChunkListRequest::Write(int fd) const {
  using T = std::decay_t<decltype(*this)>;
  static_assert(std::is_standard_layout_v<T>);
  return write(fd, this, offsetof(T, hash) + unserialized_num_hashes) == -1;
}

bool ClientStatus::Write(int fd) const {
  using T = std::decay_t<decltype(*this)>;
  static_assert(std::is_standard_layout_v<T>);
  return write(fd, this, sizeof *this) == -1;
}

bool ServerHello::Write(int fd) const {
  using T = std::decay_t<decltype(*this)>;
  static_assert(std::is_standard_layout_v<T>);
  uint8_t ulen = strlen((char *)url);
  return write(fd, this, offsetof(T, url) + ulen) == -1;
}

bool ChunkRequest::Write(int fd) const {
  using T = std::decay_t<decltype(*this)>;
  static_assert(std::is_standard_layout_v<T>);
  return write(fd, this, offsetof(T, intervals) + unserialized_num_intervals) ==
         -1;
}

bool ChunkListHeader::Write(int fd) const {
  using T = std::decay_t<decltype(*this)>;
  static_assert(std::is_standard_layout_v<T>);
  return write(fd, this, sizeof *this) == -1;
}

bool ChunkList::Write(int fd) const {
  using T = std::decay_t<decltype(*this)>;
  static_assert(std::is_standard_layout_v<T>);
  return write(fd, this, offsetof(T, hash) + unserialized_num_hashes) == -1;
}

bool Chunk::Write(int fd) const {
  using T = std::decay_t<decltype(*this)>;
  static_assert(std::is_standard_layout_v<T>);
  return write(fd, this,
               offsetof(T, data) + unserialized_data_size + start_idx) == -1;
}

} // namespace net
