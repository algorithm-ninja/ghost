#pragma once
#include <string>

class File {
public:
  File(const std::string &path, bool readonly = true);
  ~File();

  File(const File &) = delete;
  File(File &&other) {
    fd_ = other.fd_;
    len_ = other.len_;
    addr_ = other.addr_;
    other.fd_ = -1;
  }
  File &operator=(const File &) = delete;
  File &operator=(File &&) = delete;

  size_t size() const { return len_; }
  const char *data() const { return addr_; }
  char *data() { return addr_; }

private:
  int fd_;
  off_t len_;
  char *addr_;
};
