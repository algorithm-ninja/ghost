#include "net/multicast.h"
#include <gflags/gflags.h>

int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, false);
  MulticastSocket sock = JoinMulticastGroup(1234, true);
  while (true) {
    char c[1024] = {};
    sock.recv(c, sizeof c);
    fprintf(stderr, "%s\n", c);
  }
}
