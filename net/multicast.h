#include "config.h"
#include "util.h"
#include <arpa/inet.h>

class MulticastSocket : public Socket {
public:
  MulticastSocket(int fd, struct sockaddr_in6 addr) : Socket(fd), addr_(addr) {}

  bool try_recv(void *buf, size_t len) const {
    return ::recv(fd_, buf, len, 0) != -1;
  }

  void recv(void *buf, size_t len) const {
    SYSCALL_F(try_recv(buf, len), false);
  }

  bool try_send(const void *buf, size_t len) const {
    return ::sendto(fd_, buf, len, 0, (const sockaddr *)&addr_, sizeof addr_) !=
           -1;
  }

  void send(const void *buf, size_t len) const {
    SYSCALL_F(try_send(buf, len), false);
  }

private:
  struct sockaddr_in6 addr_;
};

MulticastSocket JoinMulticastGroup(int port, bool listen);
