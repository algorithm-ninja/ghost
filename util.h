#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SYSCALL_F(call, fail)                                                  \
  if ((call) == fail) {                                                        \
    perror(#call);                                                             \
    exit(1);                                                                   \
  }

#define SYSCALL(call) SYSCALL_F(call, -1)

class Socket {
public:
  explicit Socket(int fd) : fd_(fd) {}

  Socket(const Socket &other) = delete;
  Socket(Socket &&other) {
    fd_ = other.fd_;
    other.fd_ = -1;
  }

  Socket &operator=(const Socket &other) = delete;
  Socket &operator=(Socket &&other) = delete;

  operator int() const { return fd_; }

  ~Socket() {
    if (fd_ != -1) {
      close(fd_);
    }
  }

protected:
  int fd_;
};
